@echo off
echo ESP32-C3 红外接收器 - 编译和测试脚本
echo =====================================
echo.

REM 检查PlatformIO是否安装
where pio >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo 错误: 未找到PlatformIO命令行工具
    echo 请先安装PlatformIO: https://platformio.org/install/cli
    pause
    exit /b 1
)

echo 1. 清理项目...
pio run --target clean
if %ERRORLEVEL% NEQ 0 (
    echo 清理失败
    pause
    exit /b 1
)

echo.
echo 2. 编译项目...
pio run
if %ERRORLEVEL% NEQ 0 (
    echo 编译失败，请检查代码
    pause
    exit /b 1
)

echo.
echo 3. 编译成功！
echo.

REM 询问是否上传
set /p upload="是否要上传到ESP32-C3? (y/n): "
if /i "%upload%"=="y" (
    echo.
    echo 4. 上传到ESP32-C3...
    pio run --target upload
    if %ERRORLEVEL% NEQ 0 (
        echo 上传失败，请检查连接
        pause
        exit /b 1
    )
    
    echo.
    echo 5. 上传成功！
    echo.
    
    REM 询问是否监控串口
    set /p monitor="是否要打开串口监视器? (y/n): "
    if /i "%monitor%"=="y" (
        echo.
        echo 6. 打开串口监视器...
        echo 按 Ctrl+C 退出监视器
        echo.
        pio device monitor
    )
)

echo.
echo 完成！
pause
