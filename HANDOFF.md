# NekoEcat Studio — 项目交接文档

> **日期:** 2026-06-18
> **分支:** main (ahead 30 commits, 未推送)
> **远程:** https://github.com/NekoRain404/NekoEcat-Studio.git
> **测试:** 66/66 通过
> **构建:** `cmake -B build && cmake --build build -j4`

---

## 1. 项目概述

NekoEcat Studio 是一个面向 Linux + IgH EtherCAT Master 的桌面工程工作站，对标 TwinCAT。使用 C++20 / Qt6 开发。

**核心组件：**
- `ecat-studio` — Qt6 GUI 客户端（apps/ecat-studio/）
- `ecatd` — 本地 JSON-RPC 守护进程（apps/ecatd/），监听 127.0.0.1:5877
- `src/core` — 共享类型和协议（EthercatTypes, JsonProtocol, EcatService）
- `src/igh` — IgH CLI 适配层（EthercatCliBackend）

**技术栈：** C++20, Qt6 (Core/Network/Widgets/Test), CMake 3.20+, IgH EtherCAT Master API

---

## 2. 当前状态

### 已完成的工作

**架构基础设施 (Week 1)：**
- WorkspacePlugin 接口 — `apps/ecat-studio/plugins/WorkspacePlugin.h`
- PluginRegistry — `apps/ecat-studio/plugins/PluginRegistry.h/.cpp`
- EventBus (8 种事件) — `apps/ecat-studio/services/EventBus.h/.cpp`
- NotesPlugin (PoC) — `apps/ecat-studio/plugins/notes/NotesPlugin.h/.cpp`
- ThemeManager + 11 个 .qss 主题文件
- .ts/.qm 翻译系统 (7 语言)

**服务层：**
- SdoService — `apps/ecat-studio/services/SdoService.h/.cpp`
- WatchService — `apps/ecat-studio/services/WatchService.h/.cpp`
- TopologyService — `apps/ecat-studio/services/TopologyService.h/.cpp`
- DcSyncService — `apps/ecat-studio/services/DcSyncService.h/.cpp`
- AlEventService — `apps/ecat-studio/services/AlEventService.h/.cpp`
- SignalService — `apps/ecat-studio/services/SignalService.h/.cpp`

**守护进程 Handler (apps/ecatd/handlers/)：**
- DcSyncHandler — DC 同步状态查询
- AlEventHandler — AL 事件日志 (1s 轮询)
- AdapterHandler — 网卡枚举/切换
- SignalHandler — 多通道环形缓冲 (10K 点/通道)

**GUI 插件 (apps/ecat-studio/plugins/)：**
- DcSyncPlugin — DC 同步诊断工作区
- AlEventPlugin — AL 事件日志工作区
- SignalPlugin — 信号分析器 (QPainter 滚动图表)
- SignalChartWidget — 多通道实时图表

**基础设施：**
- EcatClient 自动重连 (心跳 5s + 指数退避 2→30s)
- Settings 中的网卡选择下拉框
- 12 个主题，8 语言支持

### ⚠️ 关键问题：插件系统未激活

**这是最重要的未完成工作。** 插件代码已写好但没有连接到 MainWindow：

1. PluginRegistry 存在但 MainWindow 从未调用 `registerPlugin()`
2. 新插件 (DcSync, AlEvent, Signal) 不在标签栏中显示
3. EventBus 只连接了 4 个 EcatClient 信号，缺少 dcSyncUpdate、alEvent、signalData
4. MainWindow 仍是 God Object (772 行头文件, 3335 行实现, 24K 行工作区代码)

---

## 3. 项目结构

```
NekoEcat-Studio/
├── apps/
│   ├── ecat-studio/           # GUI 客户端
│   │   ├── MainWindow.h/.cpp  # God Object (待拆解)
│   │   ├── workspaces/        # 24 个工作区文件 (24K 行)
│   │   ├── plugins/           # 插件接口和实现
│   │   │   ├── WorkspacePlugin.h   # 插件接口
│   │   │   ├── PluginRegistry.h/.cpp
│   │   │   ├── alevent/       # AL 事件插件
│   │   │   ├── dcsync/        # DC 同步插件
│   │   │   ├── notes/         # 备注插件 (PoC)
│   │   │   └── signal/        # 信号分析插件
│   │   ├── services/          # 服务层
│   │   │   ├── EventBus.h/.cpp
│   │   │   ├── SdoService.h/.cpp
│   │   │   ├── WatchService.h/.cpp
│   │   │   ├── TopologyService.h/.cpp
│   │   │   ├── DcSyncService.h/.cpp
│   │   │   ├── AlEventService.h/.cpp
│   │   │   └── SignalService.h/.cpp
│   │   ├── infra/             # 基础设施
│   │   │   ├── EcatClient.h/.cpp      # JSON-RPC 客户端
│   │   │   ├── SettingsDialog.h/.cpp   # 设置对话框
│   │   │   └── TranslationRegistry.cpp # 旧翻译系统 (2023 行)
│   │   ├── models/            # 数据模型
│   │   ├── adapters/          # 表格适配器
│   │   ├── themes/            # 11 个 .qss 主题
│   │   └── utils/             # 工具函数
│   └── ecatd/                 # 守护进程
│       ├── EcatDaemon.h/.cpp  # TCP 服务器 + handler 注册
│       ├── CommandDispatcher.h/.cpp
│       ├── FreeRunController.h/.cpp
│       ├── RtTestController.h/.cpp
│       └── handlers/          # JSON-RPC handler
│           ├── AlEventHandler.h/.cpp
│           ├── DcSyncHandler.h/.cpp
│           ├── AdapterHandler.h/.cpp
│           └── SignalHandler.h/.cpp
├── src/
│   ├── core/                  # 共享类型
│   │   ├── EthercatTypes.h    # SlaveInfo, MasterInfo
│   │   ├── JsonProtocol.h/.cpp
│   │   └── EcatService.h      # 抽象接口
│   └── igh/                   # IgH 适配
│       └── EthercatCliBackend.h/.cpp
├── tests/                     # 66 个测试
├── translations/              # 7 个 .ts 翻译文件
├── themes/                    # 11 个 .qss 主题文件
├── docs/superpowers/          # 设计文档和计划
│   ├── specs/                 # 设计规格
│   └── plans/                 # 实施计划
├── tools/                     # 工具脚本
└── .github/workflows/ci.yml   # CI 流水线
```

---

## 4. 设计文档和计划

**已批准的设计文档：**
- `docs/superpowers/specs/2026-06-18-v2-plugin-architecture-design.md` — v2 插件架构设计
- `docs/superpowers/specs/2026-06-18-v2-comprehensive-design.md` — 综合设计（拓扑+DC+ESI+总线统计）

**已批准的实施计划：**
- `docs/superpowers/plans/2026-06-18-v2-month-plan-v2.md` — 28 个任务，4 周周期

**计划概要：**
- **Week 1:** ServiceContainer + EventBus 迁移 + 4 个简单工作区迁移
- **Week 2:** 剩余 8 个工作区迁移 + MainWindow 瘦身到 <500 行
- **Week 3:** 图形拓扑 (QGraphicsScene) + DC 配置工程 UI + ESI 仓库
- **Week 4:** 总线统计 + 集成测试 + CI/CD + 文档 + v2.0.0 发布

---

## 5. 关键设计决策

### 插件架构
- 每个工作区实现 WorkspacePlugin 接口
- PluginRegistry 管理注册、排序、可见性
- EventBus 作为中央事件总线 (8 种事件类型)
- ServiceContainer 持有所有 Service 实例，注入到 Plugin 构造函数

### 守护进程
- JSON-RPC over TCP (127.0.0.1:5877)
- CommandDispatcher 分发到 handler
- 直接 ecrt API (FreeRun) + CLI fallback (其他)

### 测试
- 66 个单元测试，覆盖模型层、服务层、守护进程、GUI 冒烟
- GUI 测试使用 `QT_QPA_PLATFORM=offscreen`
- TDD 驱动开发

---

## 6. 待完成的核心工作

### 优先级 1：激活插件系统
1. 创建 ServiceContainer (已创建 .h，未完成 .cpp)
2. 将 EventBus 连接从 MainWindow 移到 ServiceContainer
3. 在 MainWindow 中注册所有插件到 PluginRegistry
4. 让 TabHost 从 PluginRegistry 驱动标签页创建

### 优先级 2：拆解 God Object
- 12 个工作区需要从 MainWindow 提取为独立 Plugin
- 详见计划中的 Tasks 3-15

### 优先级 3：新功能
- 图形拓扑可视化 (QGraphicsScene)
- DC 配置工程 UI
- ESI 仓库管理
- 总线统计仪表盘

---

## 7. 构建和测试命令

```bash
# 配置
cmake -B build

# 构建
cmake --build build -j4

# 运行所有测试
ctest --test-dir build --output-on-failure -j4

# GUI 冒烟测试
QT_QPA_PLATFORM=offscreen timeout 3 build/apps/ecat-studio/ecat-studio

# 推送到 GitHub (需要代理)
export all_proxy=http://127.0.0.1:7890
git push origin main
```

---

## 8. 已知问题

1. **插件未激活** — 代码存在但未连接到 UI
2. **MainWindow God Object** — 24K 行工作区代码待拆解
3. **TranslationRegistry 冗余** — 2023 行旧翻译系统仍在使用
4. **GitHub 推送失败** — 代理 (127.0.0.1:7890) 不可用，需要手动推送
5. **ServiceContainer.h 未提交** — 刚创建，未 add/commit

---

## 9. Git 状态

```
分支: main (ahead 30 commits, 未推送)
远程: origin → https://github.com/NekoRain404/NekoEcat-Studio.git
未提交: apps/ecat-studio/services/ServiceContainer.h (新建)
```

**最后 5 个提交：**
```
d907238 plan: v2 month plan — 28 tasks, 4 weeks
8dba5d5 spec: v2 comprehensive design
e1816fd feat: v1.0.0 release preparation
c1e333a feat: Week 3 Tasks 18-19 — Signal Handler + Signal Analyzer
d811f1a feat: Week 2 — daemon handlers + GUI plugins + auto-reconnect
```

---

## 10. 联系和上下文

- 用户语言：中文 (zh-CN)
- 开发偏好：TDD、功能优先测试 (~70% 覆盖)、激进重构策略
- 代码风格：2-space indent, `#pragma once`, 描述性注释
- 并发限制：最多 2 个子代理并行

**交接完成。祝 mimoCode 开发顺利。**
