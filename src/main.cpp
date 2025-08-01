
#include <Arduino.h>
#include <IRremote.hpp>
#include "esp32-hal-ledc.h" // Arduino HAL（Arduino 风格）
// 定义红外接收引脚
// 如果GPIO10仍有LEDC冲突，请改用GPIO18
#define IR_RECEIVE_PIN 8
#define IR_SEND_PIN 10    // 使用GPIO18避免LEDC冲突
#define BOOT_BUTTON_PIN 9 // ESP32-C3 devkitm-1 的boot按键通常为GPIO9

uint32_t lastProtocol = 0;
uint64_t lastData = 0;
uint8_t lastBits = 0;

bool hasLastSignal = false;
// 保存上一次原始脉冲数据
#define MAX_RAW_BUFFER 200 // 使用较小的缓冲区避免溢出
uint16_t lastRawBuffer[MAX_RAW_BUFFER];
uint8_t lastRawLen = 0;

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
  // 接收部分
  if (IrReceiver.decode())
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
    // 记录最后一次信号
    lastProtocol = IrReceiver.decodedIRData.protocol;
    lastData = IrReceiver.decodedIRData.decodedRawData;
    lastBits = IrReceiver.decodedIRData.numberOfBits;
    hasLastSignal = true;
    // 保存原始脉冲
    if (IrReceiver.decodedIRData.rawDataPtr)
    {
      lastRawLen = IrReceiver.decodedIRData.rawDataPtr->rawlen;
      if (lastRawLen > MAX_RAW_BUFFER)
        lastRawLen = MAX_RAW_BUFFER;
      memcpy(lastRawBuffer, IrReceiver.decodedIRData.rawDataPtr->rawbuf, lastRawLen * sizeof(uint16_t));

      // 打印原始脉冲数据
      Serial.print("原始脉冲长度: ");
      Serial.println(lastRawLen);
      Serial.print("原始脉冲数据: ");
      for (int i = 0; i < lastRawLen && i < 20; i++) // 只打印前20个数据避免过长
      {
        Serial.print(lastRawBuffer[i]);
        Serial.print(" ");
      }
      if (lastRawLen > 20)
        Serial.print("...");
      Serial.println();
    }
    IrReceiver.resume();
  }

  // 检查boot按钮，按下时发射上一次信号
  if (hasLastSignal && digitalRead(BOOT_BUTTON_PIN) == LOW)
  {
    Serial.println("检测到BOOT按钮按下，正在发射上一次接收的红外信号...");
    switch (lastProtocol)
    {

    case PULSE_DISTANCE:
    case PULSE_WIDTH:
    case UNKNOWN:
    default:
      if (lastRawLen > 0)
      {
        // 38kHz载波，rawbuf[0]是引导脉冲，rawlen为脉冲数
        Serial.print("准备发送脉冲，长度: ");
        Serial.println(lastRawLen);
        Serial.print("发送的脉冲数据: ");
        for (int i = 0; i < lastRawLen && i < 10; i++) // 打印前10个发送数据
        {
          Serial.print(lastRawBuffer[i]);
          Serial.print(" ");
        }
        if (lastRawLen > 10)
          Serial.print("...");
        Serial.println();

        IrSender.sendRaw(lastRawBuffer, lastRawLen, 38);
        ledcDetach(IR_SEND_PIN);
        Serial.println("已用sendRaw回放原始脉冲");
      }
      else
      {
        Serial.println("暂不支持该协议的发射");
      }
      break;
    }
    delay(500); // 防止长按多次发射
  }
  delay(50);
}