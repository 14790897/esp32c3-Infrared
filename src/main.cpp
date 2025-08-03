#include <Arduino.h>
#include <IRremote.hpp>
#include "esp32-hal-ledc.h"

// 定义引脚
#define IR_RECEIVE_PIN 8
#define IR_SEND_PIN 10
#define BOOT_BUTTON_PIN 9

// 存储信号序列
#define MAX_RAW_BUFFER 200
#define MAX_SIGNAL_COUNT 10

struct IRSignal
{
  uint16_t rawBuffer[MAX_RAW_BUFFER];
  uint8_t rawLen;
  unsigned long timestamp;
};

IRSignal signalSequence[MAX_SIGNAL_COUNT];
uint8_t signalCount = 0;
unsigned long lastSignalTime = 0;
const unsigned long SIGNAL_TIMEOUT = 1000; // 1秒超时
bool hasLastSignal = false;

// BOOT按钮控制
unsigned long bootPressTime = 0;
bool irReceiveEnabled = true;
const unsigned long BOOT_DISABLE_TIME = 1000;

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("ESP32-C3 红外接收器初始化中...");
  Serial.print("红外接收引脚: GPIO");
  Serial.println(IR_RECEIVE_PIN);
  Serial.print("红外发送引脚: GPIO");
  Serial.println(IR_SEND_PIN);
  Serial.print("BOOT按键引脚: GPIO");
  Serial.println(BOOT_BUTTON_PIN);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

  Serial.println("初始化完成！");
  Serial.println("等待红外信号...");
  Serial.println("按下BOOT按键可重发上一次接收的信号");
  Serial.println("========================================");
}

void loop()
{
  unsigned long currentTime = millis();

  // 检查BOOT按钮状态
  if (bootPressTime > 0 && currentTime - bootPressTime >= BOOT_DISABLE_TIME)
  {
    if (!irReceiveEnabled)
    {
      irReceiveEnabled = true;
      bootPressTime = 0;
      Serial.println("红外接收已重新启用");
    }
  }

  // 接收红外信号
  if (irReceiveEnabled && IrReceiver.decode())
  {
    Serial.print("协议: ");
    Serial.print(IrReceiver.decodedIRData.protocol);
    Serial.print(", 数据: 0x");
    Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
    Serial.print(", 位数: ");
    Serial.print(IrReceiver.decodedIRData.numberOfBits);
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
    {
      Serial.print(" (重复)");
    }
    Serial.println();
    IrReceiver.printIRResultShort(&Serial);

    // 检查是否是新的操作序列
    if (currentTime - lastSignalTime > SIGNAL_TIMEOUT || signalCount >= MAX_SIGNAL_COUNT)
    {
      signalCount = 0;
      Serial.println("====== 新的遥控操作序列开始 ======");
    }

    // 保存信号到序列中
    if (IrReceiver.decodedIRData.rawDataPtr && signalCount < MAX_SIGNAL_COUNT)
    {
      IRSignal &currentSignal = signalSequence[signalCount];
      currentSignal.rawLen = IrReceiver.decodedIRData.rawDataPtr->rawlen;
      if (currentSignal.rawLen > MAX_RAW_BUFFER)
        currentSignal.rawLen = MAX_RAW_BUFFER;

      memcpy(currentSignal.rawBuffer, IrReceiver.decodedIRData.rawDataPtr->rawbuf,
             currentSignal.rawLen * sizeof(uint16_t));
      currentSignal.timestamp = currentTime;

      Serial.print("信号 #");
      Serial.print(signalCount + 1);
      Serial.print(" 长度: ");
      Serial.print(currentSignal.rawLen);
      Serial.print(" 时间: ");
      Serial.print(currentTime);
      if (signalCount > 0)
      {
        Serial.print(" (间隔: ");
        Serial.print(currentTime - signalSequence[signalCount - 1].timestamp);
        Serial.print("ms)");
      }
      Serial.println();

      // 完整输出所有原始脉冲数据
      Serial.print("原始脉冲数据: ");
      for (int i = 0; i < currentSignal.rawLen; i++)
      {
        Serial.print(currentSignal.rawBuffer[i]);
        if (i < currentSignal.rawLen - 1)
        {
          Serial.print(", ");
        }
      }
      Serial.println();
      
      // 输出ESPHome格式的高低电平序列
      Serial.print("ESPHome格式: [");
      for (int i = 1; i < currentSignal.rawLen; i++)
      {
        uint16_t duration = currentSignal.rawBuffer[i] * 50;  // IRremote默认50微秒per tick
        if (i % 2 == 1) {
          Serial.print(duration);  // 高电平，正数
        } else {
          Serial.print("-");
          Serial.print(duration);  // 低电平，负数
        }
        if (i < currentSignal.rawLen - 1) {
          Serial.print(", ");
        }
      }
      Serial.println("]");

      signalCount++;
      hasLastSignal = true;
    }

    lastSignalTime = currentTime;
    IrReceiver.resume();
  }

  // 检查BOOT按钮，重放信号序列
  if (hasLastSignal && digitalRead(BOOT_BUTTON_PIN) == LOW)
  {
    // 禁用红外接收
    irReceiveEnabled = false;
    bootPressTime = currentTime;
    Serial.println("红外接收已暂停1秒");

    Serial.println("====== 开始重放遥控操作序列 ======");
    Serial.print("共有 ");
    Serial.print(signalCount);
    Serial.println(" 个信号需要重放");

    // 重放所有信号，保持原始时间间隔
    for (int i = 0; i < signalCount; i++)
    {
      IRSignal &signal = signalSequence[i];

      Serial.print("重放信号 #");
      Serial.print(i + 1);
      Serial.print(" 长度: ");
      Serial.print(signal.rawLen);

      if (signal.rawLen > 0)
      {
        // 等待时间间隔
        if (i > 0)
        {
          unsigned long interval = signal.timestamp - signalSequence[i - 1].timestamp;
          Serial.print(" (等待 ");
          Serial.print(interval);
          Serial.print("ms)");
          Serial.println();
          delay(interval);
        }
        else
        {
          Serial.println();
        }

        // 完整输出重放的脉冲数据
        Serial.print("重放原始脉冲: ");
        for (int j = 0; j < signal.rawLen; j++)
        {
          Serial.print(signal.rawBuffer[j]);
          if (j < signal.rawLen - 1)
          {
            Serial.print(", ");
          }
        }
        Serial.println();
        
        // 输出ESPHome格式的重放序列
        Serial.print("重放ESPHome格式: [");
        for (int j = 1; j < signal.rawLen; j++)
        {
          uint16_t duration = signal.rawBuffer[j] * 50;  // IRremote默认50微秒per tick
          if (j % 2 == 1) {
            Serial.print(duration);  // 高电平，正数
          } else {
            Serial.print("-");
            Serial.print(duration);  // 低电平，负数
          }
          if (j < signal.rawLen - 1) {
            Serial.print(", ");
          }
        }
        Serial.println("]");

        // 发送信号
        Serial.print("发送中...");
        IrSender.sendRaw(signal.rawBuffer, signal.rawLen, 38);
        ledcDetach(IR_SEND_PIN);
        Serial.println(" 完成");
      }
    }

    Serial.println("====== 信号序列重放完成 ======");
    delay(1000); // 防止长按多次发射
  }

  delay(50);
}