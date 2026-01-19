# Development Tools

本目录包含开发工具的配置文件。

## 文件说明

### .clangd

Clangd LSP（Language Server Protocol）配置文件，用于代码补全和语法检查。

**使用方法**：
1. 如果你使用支持 clangd 的 IDE（如 VSCode、Neovim 等）
2. 将此文件复制到项目根目录：
   ```bash
   cp docs/dev-tools/.clangd .
   ```
3. 重启你的 IDE 即可获得代码补全功能

**注意**：此文件已添加到 `.gitignore`，不会被提交到仓库。每个开发者可以根据自己的需求配置。

## 为什么放在这里？

这些文件是个人开发环境配置，不是项目必需的文件。放在 `docs/dev-tools/` 可以：
- 保持项目根目录整洁
- 作为参考供其他开发者使用
- 避免不同开发者的配置冲突
