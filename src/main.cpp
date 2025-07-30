#include <Arduino.h>
#include "ir_receiver.h"

// 创建红外接收器对象（使用默认配置GPIO2）
IRReceiver irReceiver;

void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  while (!Serial) {
    delay(50);
  }

  Serial.println();
  Serial.println("ESP32-C3 红外接收器初始化中...");

  // 初始化红外接收器
  irReceiver.begin();
  Serial.println("等待红外信号...");
  Serial.println("========================================");
}

void loop() {
  // 检查是否接收到红外信号
  if (irReceiver.available()) {
    // 打印接收到的信号详细信息
    irReceiver.printSignalInfo();

    // 恢复接收器以接收下一个信号
    irReceiver.resume();
  }

  // 短暂延时以避免过度占用CPU
  delay(50);
}