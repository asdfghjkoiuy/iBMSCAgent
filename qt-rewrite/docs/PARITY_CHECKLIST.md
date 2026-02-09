# iBMSC VB -> Qt Parity Checklist

目标：逐项对齐 `iBMSC/iBMSC/*.vb` 的功能与交互，不做“只像不像”。

## 1. 主窗口与布局（MainWindow.designer.vb）
- [x] 基础主窗口、菜单、工具栏、谱面主区
- [x] Header/WAV/BMP/Beat/Expansion 选项区
- [ ] 完整还原 PO 系列折叠/伸缩行为（POHeader/POGrid/POWaveForm/POWAV/POBMP/POBeat/POExpansion）
- [x] 三分屏编辑区（PMainL/PMain/PMainR）及滚动联动

## 2. BMS 解析与保存（ChartIO.vb, BMS.vb）
- [x] Header 关键字段
- [x] #WAV/#BMP/#BPMxx/#STOPxx/#SCROLLxx
- [x] 常用通道 note line 解析和回写
- [x] Expansion 保留与回写
- [ ] BMSE/NT 输入细节完全一致（LN、重叠策略、边界）
- [ ] 复杂导入导出（SM/ibmsc）完整对齐

## 3. 谱面编辑核心（PanelEvents.vb, PanelDraw.vb）
- [x] 点击新增/删除音符
- [x] Ctrl+滚轮缩放
- [x] mac 触摸板平滑滚动/惯性滚动/Pinch 缩放兼容
- [x] 原版选择工具基础（框选、多选修饰键）
- [ ] 原版写入工具（长音、隐藏、地雷、自动 wav increase）
- [x] 时间选区工具基础（可视化选区）
- [x] 键盘映射基础（上下左右、Delete、Ctrl+A）
- [ ] 完整绘制样式（NT/BMSE 双模式、鼠标悬停、列高亮、波形叠加）

## 4. 资源编辑与预览（Audio.vb, Waveform.vb）
- [x] WAV/OGG 表编辑与试听（wav/ogg fallback）
- [x] BMP/BGI 表编辑与图片预览
- [ ] 波形面板参数与导入（透明度/宽度/偏移/精度/锁定）
- [ ] 视频类 BGI 预览与行为对齐

## 5. 皮肤/视觉系统（VisualSettings.vb, Theme XML）
- [x] Theme XML 加载并应用列颜色/宽度/背景
- [ ] 视觉参数全量映射（字体、笔刷、k 系列参数、时间选区样式）
- [ ] 主题保存与兼容导入

## 6. 编辑器系统能力（UndoRedo.vb, Editor*.vb）
- [x] Undo/Redo 基础快照系统
- [x] Recent Files/Persistent Settings
- [x] Find/Statistics 基础工具入口
- [x] Replace 基础工具入口
- [ ] MyO2/Conversion 工具
- [x] AutoSave/ErrorCheck/PreviewOnClick 基础选项

## 7. 质量与验证
- [ ] 以原版样例谱面做回归对比（读->写差异）
- [ ] 行为测试：鼠标/键盘/触摸板一致性
- [ ] 大谱面性能测试
