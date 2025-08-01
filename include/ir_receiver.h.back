#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

// 红外接收器配置
struct IRConfig {
    uint16_t recvPin;           // 接收引脚
    uint16_t bufferSize;        // 缓冲区大小
    uint8_t timeout;            // 超时时间(ms)
    bool enablePullup;          // 是否启用内部上拉电阻
    bool invertSignal;          // 是否反转信号电平
};

// 默认配置
const IRConfig DEFAULT_IR_CONFIG = {
    .recvPin = 9,               // GPIO9 (可修改为其他引脚，如3、4、5等)
    .bufferSize = 1024,         // 1KB缓冲区
    .timeout = 15,              // 15ms超时
    .enablePullup = true,       // 启用上拉电阻
    .invertSignal = true        // 反转信号电平（适用于大多数红外接收模块）
};

// 红外接收器类
class IRReceiver {
private:
    IRrecv* irrecv;
    IRConfig config;
    decode_results results;
    unsigned long lastSignalTime;
    bool signalGroupActive;
    int signalCount;

public:
    // 构造函数
    IRReceiver(const IRConfig& cfg = DEFAULT_IR_CONFIG);
    
    // 析构函数
    ~IRReceiver();
    
    // 初始化接收器
    void begin();
    
    // 检查是否有新的红外信号
    bool available();
    
    // 获取解码结果
    decode_results getResults();
    
    // 恢复接收器以接收下一个信号
    void resume();
    
    // 打印信号详细信息
    void printSignalInfo();
    
    // 获取协议名称
    String getProtocolName();
    
    // 获取信号数据
    uint64_t getSignalData();
    
    // 获取信号位数
    uint16_t getSignalBits();
    
    // 是否为重复信号
    bool isRepeat();
    
    // 停用接收器
    void end();

    // 检查信号组是否超时（超过1秒）
    void checkSignalGroupTimeout();

    // 开始新的信号组
    void startNewSignalGroup();

    // 结束当前信号组
    void endSignalGroup();
};

#endif // IR_RECEIVER_H
