# XDock

**独立的经典桌面 Dock，与 [XLaunch](https://github.com/ai-workspace-lab/XLaunch) 配合使用。**

[English](README.md) · [设计说明](docs/design-brief.md)

XDock 从用户提供的 2013 DDE 截图中拆分底部栏：负责固定应用、运行状态和窗口切换。XLaunch 负责分类、应用搜索、系统动作与 Agent 指令。两个独立仓库可分别开发、发布与启用。

当前状态：设计规格，尚无可执行 Dock，尚未验证平台兼容性。

![用户提供的 2013 DDE 参考图](assets/dde-2013-reference.png)

视觉沿用原图底部的贴边布局、紧凑彩色图标、轻薄深色透明底栏，与琥珀色桌面搭配。截图中的全屏启动器属于 XLaunch。

XDock 独立运行，不要求 XLaunch 或 Agent 服务；XLaunch 可作为可选固定入口。目标平台为 macOS 26+、GNOME 50、KDE Plasma 6.6、DDE 7.0、Xfce，均待验证。不依赖 DDE 全家桶。

下一步：基于原图设计独立 Dock 的各交互状态，再验证平台适配能力。
