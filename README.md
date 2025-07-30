# ESP32-C3 红外接收模块

这是一个基于ESP32-C3的红外接收器项目，使用IRremoteESP8266库来接收和解码各种红外遥控器信号。

## 功能特性

- 支持多种红外协议（NEC、SONY、RC5、Samsung等）
- 可配置的接收引脚和参数
- 详细的信号信息显示
- 面向对象的设计，易于集成和扩展
- 支持重复信号检测
- 原始时序数据输出

## 硬件连接

### 基本连接
- 红外接收器模块的VCC连接到ESP32-C3的3.3V
- 红外接收器模块的GND连接到ESP32-C3的GND
- 红外接收器模块的OUT连接到ESP32-C3的GPIO2（默认，可配置）

### 推荐的红外接收器模块
- VS1838B
- TSOP4838
- IRM-3638T

## 软件配置

### 依赖库
项目使用以下库：
- IRremoteESP8266 (v2.8.6+)

### 默认配置
```cpp
const IRConfig DEFAULT_IR_CONFIG = {
    .recvPin = 2,               // GPIO2
    .bufferSize = 1024,         // 1KB缓冲区
    .timeout = 15,              // 15ms超时
    .enablePullup = true        // 启用上拉电阻
};
```

## 使用方法

### 基本使用
```cpp
#include <Arduino.h>
#include "ir_receiver.h"

// 创建红外接收器对象（使用默认配置）
IRReceiver irReceiver;

void setup() {
    Serial.begin(115200);
    irReceiver.begin();
}

void loop() {
    if (irReceiver.available()) {
        // 打印详细信号信息
        irReceiver.printSignalInfo();
        
        // 获取信号数据
        String protocol = irReceiver.getProtocolName();
        uint64_t data = irReceiver.getSignalData();
        
        // 恢复接收器
        irReceiver.resume();
    }
    delay(100);
}
```

### 自定义配置
```cpp
// 自定义配置
IRConfig customConfig = {
    .recvPin = 3,           // 使用GPIO3
    .bufferSize = 2048,     // 2KB缓冲区
    .timeout = 20,          // 20ms超时
    .enablePullup = true
};

IRReceiver customIrReceiver(customConfig);
```

## API参考

### IRReceiver类方法

- `IRReceiver(const IRConfig& cfg = DEFAULT_IR_CONFIG)` - 构造函数
- `void begin()` - 初始化接收器
- `bool available()` - 检查是否有新信号
- `decode_results getResults()` - 获取原始解码结果
- `void resume()` - 恢复接收器
- `void printSignalInfo()` - 打印详细信号信息
- `String getProtocolName()` - 获取协议名称
- `uint64_t getSignalData()` - 获取信号数据
- `uint16_t getSignalBits()` - 获取信号位数
- `bool isRepeat()` - 检查是否为重复信号
- `void end()` - 停用接收器

### IRConfig结构体

```cpp
struct IRConfig {
    uint16_t recvPin;           // 接收引脚
    uint16_t bufferSize;        // 缓冲区大小
    uint8_t timeout;            // 超时时间(ms)
    bool enablePullup;          // 是否启用内部上拉电阻
};
```

## 编译和上传

1. 确保已安装PlatformIO
2. 在项目目录中运行：
   ```bash
   pio run
   ```
3. 上传到ESP32-C3：
   ```bash
   pio run --target upload
   ```
4. 监控串口输出：
   ```bash
   pio device monitor
   ```

## 故障排除

### 常见问题

1. **无法接收信号**
   - 检查硬件连接
   - 确认红外接收器模块工作正常
   - 检查引脚配置是否正确

2. **信号解码失败**
   - 尝试增加缓冲区大小
   - 调整超时时间
   - 检查红外遥控器是否工作正常

3. **重复信号过多**
   - 这是正常现象，长按遥控器按键会产生重复信号
   - 可以通过`isRepeat()`方法过滤重复信号

## 许可证

本项目采用MIT许可证。
