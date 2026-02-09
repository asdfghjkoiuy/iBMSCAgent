# iBMSC Qt Rewrite (CMake + C++17 + Qt6)

该目录是对 `iBMSC` Visual Basic 工程的 C++/Qt 重写起步版本，目标是逐步替代原 WinForms 实现。

## 当前已实现功能

- BMS 基础解析和保存
  - Header: `#TITLE/#ARTIST/#GENRE/#BPM/#PLAYER/#RANK/#PLAYLEVEL/#DIFFICULTY/#TOTAL/...`
  - 资源表: `#WAVxx/#BMPxx/#BPMxx/#STOPxx/#SCROLLxx`
  - 通道数据: `#mmmcc:...`（包括常见按键/BGM/BPM/STOP/SCROLL 通道）
  - 扩展行保留（未知/条件编译行进入 expansion 文本并在保存时回写）
- 谱面编辑
  - 三分屏联动编辑区（滚动联动）
  - 轨道可视化绘制
  - 左键新增或选中音符，右键删除音符
  - `Ctrl + 滚轮` 缩放纵向视图
  - mac 触摸板两指平滑滚动、惯性滚动、Pinch 缩放
  - 选择模式（框选/多选）、写入模式、时间选区模式
  - 键盘编辑基础（方向键移动、Delete 删除、Ctrl+A 全选）
  - Undo/Redo（快照版）
  - 可设置写入通道与写入值
- 皮肤系统
  - 读取 `*.Theme.xml`（兼容 iBMSC 主题结构）
  - 支持切换并即时应用列宽/列色/网格背景到编辑器
- BGI 编辑
  - `#BMPxx` 表格编辑
  - 图片类 BGI 预览
- Beat/Expansion 编辑
  - Measure 比例（`#xxx02:`）表格编辑
  - Expansion 原始文本编辑并保存回写
- 编辑器能力
  - Recent Files
  - 持久化设置（窗口状态、Snap、模式）
  - Statistics / Find By Value
  - AutoSave / ErrorCheck / PreviewOnClick 基础选项
- WAV/OGG 编辑
  - `#WAVxx` 表格编辑
  - 音频试听（自动在 `.wav`/`.ogg` 间回退查找）

## 构建

要求：Qt 6（Widgets/Multimedia/Xml）和 CMake >= 3.20

```bash
cd qt-rewrite
cmake -S . -B build
cmake --build build -j
./build/iBMSCQt
```

## 与原版差异（第一版）

- 已覆盖核心流程：读取 -> 编辑 -> 主题切换 -> 资源编辑 -> 保存。
- 仍在补齐高级编辑行为（批量操作、复杂 LN 配对、完整 Undo/Redo、所有对话框和高级工具链）。
