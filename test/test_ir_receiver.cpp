/*
 * ESP32-C3 红外接收器测试文件
 * 
 * 此文件包含基本的功能测试
 */

#include <Arduino.h>
#include "ir_receiver.h"

// 测试用的红外接收器
IRReceiver testReceiver;

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
    
    // 测试初始化
    testReceiver.begin();
    
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
        testReceiver.end();
        while (true) {
            delay(1000);
        }
    }
    
    // 检查红外信号
    if (testReceiver.available()) {
        signalCount++;
        lastSignalTime = currentTime;
        
        Serial.print("信号 #");
        Serial.print(signalCount);
        Serial.print(" - ");
        
        // 获取基本信息
        String protocol = testReceiver.getProtocolName();
        uint64_t data = testReceiver.getSignalData();
        uint16_t bits = testReceiver.getSignalBits();
        bool repeat = testReceiver.isRepeat();
        
        // 简化输出
        Serial.print("协议: ");
        Serial.print(protocol);
        Serial.print(", 数据: 0x");
        Serial.print((uint32_t)data, HEX);
        Serial.print(", 位数: ");
        Serial.print(bits);
        
        if (repeat) {
            Serial.print(" [重复]");
        }
        Serial.println();
        
        // 测试各个方法
        testMethods();
        
        testReceiver.resume();
    }
    
    // 显示测试进度
    static unsigned long lastProgressTime = 0;
    if (currentTime - lastProgressTime > 10000) { // 每10秒显示一次进度
        lastProgressTime = currentTime;
        int remainingSeconds = (60000 - (currentTime - testStartTime)) / 1000;
        Serial.print("测试进行中... 剩余时间: ");
        Serial.print(remainingSeconds);
        Serial.print("秒, 已接收信号: ");
        Serial.println(signalCount);
    }
    
    delay(50);
}

// 测试各个方法的功能
void testMethods() {
    // 测试获取解码结果
    decode_results results = testReceiver.getResults();
    
    // 验证数据一致性
    if (results.value == testReceiver.getSignalData() &&
        results.bits == testReceiver.getSignalBits() &&
        results.repeat == testReceiver.isRepeat()) {
        Serial.println("  ✓ 方法测试通过");
    } else {
        Serial.println("  ✗ 方法测试失败");
    }
}

// 测试自定义配置（可选）
void testCustomConfig() {
    IRConfig testConfig = {
        .recvPin = 2,
        .bufferSize = 512,
        .timeout = 10,
        .enablePullup = true
    };
    
    IRReceiver customReceiver(testConfig);
    customReceiver.begin();
    
    Serial.println("自定义配置测试完成");
    
    customReceiver.end();
}
