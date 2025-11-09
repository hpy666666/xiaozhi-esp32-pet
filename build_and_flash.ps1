# 表情模块编译和烧录脚本
# 使用方法: 在ESP-IDF 5.3 PowerShell中运行此脚本

Write-Host "=== 小智宠物表情模块 - 编译烧录脚本 ===" -ForegroundColor Green

# 切换到项目目录
$ProjectDir = "C:\Users\34376\Desktop\xiaozhi-esp32-1.9.2_2\xiaozhi-esp32-1.9.2_2"
Set-Location $ProjectDir

Write-Host "`n[1/4] 删除旧的构建文件..." -ForegroundColor Yellow
Remove-Item "releases\v1.9.2_xiaozhi-pet.zip" -ErrorAction SilentlyContinue
if ($?) {
    Write-Host "  ✓ 已删除旧zip文件" -ForegroundColor Green
} else {
    Write-Host "  - 未找到旧zip文件（可能是首次编译）" -ForegroundColor Gray
}

Write-Host "`n[2/4] 编译xiaozhi-pet固件..." -ForegroundColor Yellow
python scripts\release.py xiaozhi-pet
if ($LASTEXITCODE -ne 0) {
    Write-Host "  ✗ 编译失败！请检查错误信息" -ForegroundColor Red
    exit 1
}
Write-Host "  ✓ 编译成功" -ForegroundColor Green

Write-Host "`n[3/4] 烧录固件到COM5..." -ForegroundColor Yellow
idf.py -p COM5 flash
if ($LASTEXITCODE -ne 0) {
    Write-Host "  ✗ 烧录失败！请检查设备连接" -ForegroundColor Red
    exit 1
}
Write-Host "  ✓ 烧录成功" -ForegroundColor Green

Write-Host "`n[4/4] 启动串口监视器..." -ForegroundColor Yellow
Write-Host "  提示: 按 Ctrl+] 退出监视器" -ForegroundColor Cyan
Write-Host ""
idf.py -p COM5 monitor
