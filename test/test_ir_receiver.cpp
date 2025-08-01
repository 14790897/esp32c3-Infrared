/*
 * ESP32-C3 红外接收器测试文件
 * 
 * 此文件包含基本的功能测试
 */

#include <Arduino.h>
#define SEND_PWM_BY_TIMER false // 禁用LEDC PWM，使用软件PWM
#define USE_SOFTWARE_PWM true   // 强制使用软件PWM
#include <IRremote.hpp>

// 定义红外接收引脚
#define IR_RECEIVE_PIN 8
#define IR_SEND_PIN 10

// 测试计数器
int signalCount = 0;
unsigned long lastSignalTime = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(50);
    }
    
    Serial.println("ESP32-C3 红外接收器测试");
    Serial.println("=======================");

    // 初始化红外接收器
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.println("测试开始 - 请使用红外遥控器发送信号");
    Serial.println("测试将运行60秒...");
    Serial.println();
}

void loop() {
    static unsigned long testStartTime = millis();
    unsigned long currentTime = millis();
    
    // 测试运行60秒
    if (currentTime - testStartTime > 60000) {
        Serial.println();
        Serial.println("测试完成！");
        Serial.print("总共接收到 ");
        Serial.print(signalCount);
        Serial.println(" 个信号");
        
        if (signalCount > 0) {
            Serial.println("✓ 红外接收器工作正常");
        } else {
            Serial.println("✗ 未接收到任何信号，请检查硬件连接");
        }

        // 停止测试
        while (true) {
            delay(1000);
        }
    }
    
    // 检查红外信号
    if (IrReceiver.decode())
    {
        signalCount++;
        lastSignalTime = currentTime;

        Serial.print("信号 #");
        Serial.print(signalCount);
        Serial.print(" - ");

        // 获取基本信息
        Serial.print("协议: ");
        Serial.print(IrReceiver.decodedIRData.protocol);
        Serial.print(", 数据: 0x");
        Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
        Serial.print(", 位数: ");
        Serial.print(IrReceiver.decodedIRData.numberOfBits);

        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
        {
            Serial.print(" [重复]");
        }
        Serial.println();

        // 打印详细信息
        IrReceiver.printIRResultShort(&Serial);

        IrReceiver.resume();
    }

    // 显示测试进度
    static unsigned long lastProgressTime = 0;
    if (currentTime - lastProgressTime > 10000) { // 每10秒显示一次进度
        lastProgressTime = currentTime;
        int remainingSeconds = (60000 - (currentTime - testStartTime)) / 1000;
        Serial.print("测试进行中... 剩余时间: ");
        Serial.print(remainingSeconds);
        Serial.print("秒, 已接收信号: ");
        Serial.print(signalCount);
        Serial.println(" 个");
    }
    
    delay(50);
}
