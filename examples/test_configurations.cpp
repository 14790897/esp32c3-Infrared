/*
 * 红外接收器测试配置示例
 * 
 * 如果您的红外接收器无法正常工作，可以尝试以下不同的配置
 */

#include <Arduino.h>
#include "ir_receiver.h"

// 配置1：标准红外接收模块（如VS1838B、TSOP4838）
// 这些模块通常需要信号反转
const IRConfig STANDARD_MODULE_CONFIG = {
    .recvPin = 9,               // GPIO9
    .bufferSize = 1024,         // 1KB缓冲区
    .timeout = 15,              // 15ms超时
    .enablePullup = true,       // 启用上拉电阻
    .invertSignal = true        // 反转信号（推荐）
};

// 配置2：如果标准配置不工作，尝试不反转信号
const IRConfig NO_INVERT_CONFIG = {
    .recvPin = 9,               // GPIO9
    .bufferSize = 1024,         // 1KB缓冲区
    .timeout = 15,              // 15ms超时
    .enablePullup = true,       // 启用上拉电阻
    .invertSignal = false       // 不反转信号
};

// 配置3：禁用上拉电阻（如果外部已有上拉电阻）
const IRConfig EXTERNAL_PULLUP_CONFIG = {
    .recvPin = 9,               // GPIO9
    .bufferSize = 1024,         // 1KB缓冲区
    .timeout = 15,              // 15ms超时
    .enablePullup = false,      // 禁用内部上拉电阻
    .invertSignal = true        // 反转信号
};

// 配置4：高灵敏度配置（更大缓冲区，更短超时）
const IRConfig HIGH_SENSITIVITY_CONFIG = {
    .recvPin = 9,               // GPIO9
    .bufferSize = 2048,         // 2KB缓冲区
    .timeout = 10,              // 10ms超时
    .enablePullup = true,       // 启用上拉电阻
    .invertSignal = true        // 反转信号
};

/*
 * 使用方法：
 * 
 * 1. 在main.cpp中替换DEFAULT_IR_CONFIG为以上任一配置
 * 2. 例如：IRReceiver irReceiver(STANDARD_MODULE_CONFIG);
 * 3. 如果仍然无法工作，尝试其他配置
 * 
 * 常见问题排查：
 * - 如果完全没有信号：检查接线和电源
 * - 如果有信号但解码错误：尝试切换invertSignal设置
 * - 如果信号不稳定：尝试调整timeout值或缓冲区大小
 */
