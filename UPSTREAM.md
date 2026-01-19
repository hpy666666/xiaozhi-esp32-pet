# Upstream Project Information

## 原项目信息

本项目 Fork 自 **xiaozhi-esp32** 开源项目。

### 项目链接

- **GitHub 仓库**: https://github.com/78/xiaozhi-esp32
- **作者**: 78 / 虾哥
- **QQ 交流群**: 1011329060

### Fork 原因

xiaozhi-esp32 是一个优秀的多板支持的 ESP32 语音助手项目，支持 88+ 种不同的开发板。

本项目专注于 **xiaozhi-pet（桌面宠物机器人）** 版本的开发，为了：
1. **简化代码库** - 只保留宠物版相关代码，便于维护和开发
2. **专注特性开发** - 集中精力开发舵机控制、动态表情等宠物机器人特性
3. **独立硬件设计** - 提供专门的原理图和PCB设计
4. **清晰的项目定位** - 让用户一眼就能看出这是宠物机器人项目

### 主要改进

相比原版 xiaozhi-esp32，本项目增加了：
- ✅ 4个舵机驱动和13种预定义动作
- ✅ 慵懒眨眼GIF动画系统（4种专业曲线）
- ✅ 18种静态位图表情
- ✅ 舵机分时启动优化（解决电流冲击问题）
- ✅ 完整的硬件设计文件（原理图、PCB）
- ✅ 详细的动画制作工具和文档

### 如何同步上游更新

如果你想将原项目的更新同步到本项目，可以参考以下步骤：

#### 1. 添加上游仓库

```bash
cd xiaozhi-esp32-pet
git remote add upstream https://github.com/78/xiaozhi-esp32.git
git fetch upstream
```

#### 2. 查看上游更新

```bash
git log HEAD..upstream/master --oneline
```

#### 3. 选择性合并更新

```bash
# 查看具体改动
git diff upstream/master

# 创建新分支进行合并
git checkout -b sync-upstream
git merge upstream/master

# 解决冲突后提交
git add .
git commit -m "Sync updates from upstream"
```

#### 4. 注意事项

由于本项目已经进行了以下改动，合并时需要特别注意：
- `main/boards/` 只保留了 `xiaozhi-pet` 和 `common`，其他板子在 `docs/other-boards/`
- 增加了 `main/pet/` 舵机控制模块
- 增加了 `main/display/` 的动态表情系统
- 文档结构已重组到 `docs/` 目录

建议：
1. 只合并 `main/audio/`、`main/network/` 等公共模块的改进
2. 不要合并新增的板子配置（除非你需要）
3. 注意保留本项目特有的宠物功能代码

### 其他开发板配置

如果你需要使用原项目支持的其他开发板：
1. 查看 [`docs/other-boards/`](docs/other-boards/) 目录
2. 或直接访问原项目: https://github.com/78/xiaozhi-esp32

### 致谢

感谢 xiaozhi-esp32 项目及其作者 78/虾哥 提供的优秀开源基础！

---

**最后更新**: 2026-01-19
