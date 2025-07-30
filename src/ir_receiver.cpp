#include "ir_receiver.h"

// 构造函数
IRReceiver::IRReceiver(const IRConfig& cfg) : config(cfg) {
    irrecv = new IRrecv(config.recvPin, config.bufferSize, config.timeout, config.enablePullup);
    lastSignalTime = 0;
    signalGroupActive = false;
    signalCount = 0;
}

// 析构函数
IRReceiver::~IRReceiver() {
    if (irrecv) {
        delete irrecv;
        irrecv = nullptr;
    }
}

// 初始化接收器
void IRReceiver::begin() {
    if (irrecv) {
        irrecv->enableIRIn();

        // 设置信号反转
        if (config.invertSignal) {
            irrecv->setTolerance(kTolerance);  // 使用默认容差
            // 注意：IRremoteESP8266库会自动处理信号反转
        }

        Serial.println("红外接收器已启动");
        Serial.print("接收引脚: GPIO");
        Serial.println(config.recvPin);
        Serial.print("缓冲区大小: ");
        Serial.println(config.bufferSize);
        Serial.print("超时时间: ");
        Serial.print(config.timeout);
        Serial.println("ms");
        Serial.print("信号反转: ");
        Serial.println(config.invertSignal ? "启用" : "禁用");
        Serial.print("上拉电阻: ");
        Serial.println(config.enablePullup ? "启用" : "禁用");
    }
}

// 检查是否有新的红外信号
bool IRReceiver::available() {
    // 检查信号组超时
    checkSignalGroupTimeout();

    if (irrecv && irrecv->decode(&results)) {
        unsigned long currentTime = millis();

        // 如果没有活跃的信号组，开始新的信号组
        if (!signalGroupActive) {
            startNewSignalGroup();
        }

        // 更新最后信号时间
        lastSignalTime = currentTime;
        signalCount++;

        return true;
    }
    return false;
}

// 获取解码结果
decode_results IRReceiver::getResults() {
    return results;
}

// 恢复接收器以接收下一个信号
void IRReceiver::resume() {
    if (irrecv) {
        irrecv->resume();
    }
}

// 打印信号详细信息
void IRReceiver::printSignalInfo() {
    Serial.print("信号 #");
    Serial.print(signalCount);
    Serial.print(" - 协议: ");
    Serial.print(getProtocolName());
    Serial.print(", 数据: 0x");
    serialPrintUint64(results.value, HEX);
    Serial.print(", 位数: ");
    Serial.print(results.bits);

    if (isRepeat()) {
        Serial.print(" (重复)");
    }
    Serial.println();

    // 打印完整的原始数据
    Serial.print("  原始数据 (");
    Serial.print(results.rawlen);
    Serial.print("): ");

    // 打印所有原始时序数据，用正负号表示高低电平
    for (uint16_t i = 1; i < results.rawlen; i++) {
        uint16_t duration = results.rawbuf[i] * kRawTick;

        // 奇数索引为高电平(正数)，偶数索引为低电平(负数)
        if (i % 2 == 1) {
            Serial.print(duration);      // 高电平，正数
        } else {
            Serial.print("-");
            Serial.print(duration);      // 低电平，负数
        }

        if (i < results.rawlen - 1) {
            Serial.print(", ");
        }
    }
    Serial.println();
}

// 获取协议名称
String IRReceiver::getProtocolName() {
    return String(typeToString(results.decode_type));
}

// 获取信号数据
uint64_t IRReceiver::getSignalData() {
    return results.value;
}

// 获取信号位数
uint16_t IRReceiver::getSignalBits() {
    return results.bits;
}

// 是否为重复信号
bool IRReceiver::isRepeat() {
    return results.repeat;
}

// 停用接收器
void IRReceiver::end() {
    if (irrecv) {
        irrecv->disableIRIn();
        Serial.println("红外接收器已停用");
    }
}

// 检查信号组是否超时（超过1秒）
void IRReceiver::checkSignalGroupTimeout() {
    if (signalGroupActive && (millis() - lastSignalTime > 1000)) {
        endSignalGroup();
    }
}

// 开始新的信号组
void IRReceiver::startNewSignalGroup() {
    signalGroupActive = true;
    signalCount = 0;
    Serial.println("========================================");
    Serial.println("开始接收红外信号组...");
}

// 结束当前信号组
void IRReceiver::endSignalGroup() {
    if (signalGroupActive) {
        signalGroupActive = false;
        Serial.println("----------------------------------------");
        Serial.print("信号组接收完成，共接收到 ");
        Serial.print(signalCount);
        Serial.println(" 个信号");
        Serial.println("========================================");
        Serial.println();
    }
}
