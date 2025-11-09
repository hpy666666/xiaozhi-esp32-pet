@echo off
echo ========================================
echo Xiaozhi Pet Board - 固件生成脚本
echo ========================================
echo.

cd /d %~dp0

echo [1/4] 设置目标芯片为 ESP32-S3...
call idf.py set-target esp32s3
if %errorlevel% neq 0 (
    echo 错误: 设置目标失败
    pause
    exit /b 1
)

echo.
echo [2/4] 编译项目...
call idf.py -DBOARD_NAME=xiaozhi-pet build
if %errorlevel% neq 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo.
echo [3/4] 合并固件...
call python scripts\release.py xiaozhi-pet
if %errorlevel% neq 0 (
    echo 错误: 合并固件失败
    pause
    exit /b 1
)

echo.
echo [4/4] 完成!
echo ========================================
echo 固件已生成到: releases\目录
echo.
echo 请使用烧录工具烧录 merged-binary.bin 到 0x0 地址
echo 或者运行: idf.py -p COMx flash
echo ========================================
pause
