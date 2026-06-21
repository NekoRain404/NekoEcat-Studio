# NekoEcat Studio — 项目交接文档

> **日期:** 2026-06-19
> **分支:** main
> **远程:** https://github.com/NekoRain404/NekoEcat-Studio.git
> **测试:** 111/111 通过
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

**架构基础设施：**
- WorkspacePlugin 接口 — `apps/ecat-studio/plugins/WorkspacePlugin.h`
- PluginRegistry — `apps/ecat-studio/plugins/PluginRegistry.h/.cpp`
- EventBus (8 种事件) — `apps/ecat-studio/services/EventBus.h/.cpp`
- ServiceContainer (33 个服务) — `apps/ecat-studio/services/ServiceContainer.h/.cpp`

**服务层 (33 个服务)：**
- 核心服务：EcatClient, EventBus, SdoService, WatchService, TopologyService
- 硬件服务：DcSyncService, AlEventService, SignalService, BusStatsService
- 监控服务：PerformanceMonitorService, WatchdogService, EcatHealthService, NetworkDiagnosticsService
- 安全服务：SafetyController, AlarmService
- 数据服务：EsiService, ProjectManagerService, ConfigurationService, DataPipelineService, DeviceManagerService
- 报告服务：DiagnosticReportService, LoggingService, ChartService, ExportService, ReportGeneratorService
- 操作服务：BatchOperationService, AsyncOperationManager, FirmwareUpdateService
- 可选服务：ScriptingService (需要 ECAT_SCRIPTING_ENABLED)

**GUI 插件 (30 个插件)：**
- 核心工作区：Overview, Topology, Object Dictionary, Watch, FreeRun, StartupSdo, IoVariable
- 诊断工作区：Consistency, Diagnostics, DC Sync, AL Events, Signal Analyzer, Network Adapter
- 高级工作区：Dashboard, Chart, Oscilloscope, Protocol Analyzer, Automation, Alarm
- 管理工作区：Session, Export, Notes, ESI Repository, Bus Statistics, Project
- 新增工作区：DataPipeline, DeviceManager, MasterManager

**守护进程 Handler (apps/ecatd/handlers/)：**
- DcSyncHandler — DC 同步状态查询
- AlEventHandler — AL 事件日志 (1s 轮询)
- AdapterHandler — 网卡枚举/切换
- SignalHandler — 多通道环形缓冲 (10K 点/通道)

**基础设施：**
- EcatClient 自动重连 (心跳 5s + 指数退避 2→30s)
- Settings 中的网卡选择下拉框
- 12 个主题，8 语言支持
- LRU 缓存支持 TTL 过期
- 异步操作管理器支持优先级队列
- 内存池减少频繁分配开销

---

## 3. 项目结构

```
NekoEcat-Studio/
├── apps/
│   ├── ecat-studio/           # GUI 客户端
│   │   ├── MainWindow.h/.cpp  # 主窗口
│   │   ├── workspaces/        # 31 个工作区文件
│   │   ├── plugins/           # 插件接口和实现 (30 个插件)
│   │   ├── services/          # 服务层 (33 个服务)
│   │   ├── infra/             # 基础设施
│   │   ├── models/            # 数据模型
│   │   ├── adapters/          # 表格适配器
│   │   ├── themes/            # 12 个 .qss 主题
│   │   └── utils/             # 工具函数
│   └── ecatd/                 # 守护进程
│       ├── EcatDaemon.h/.cpp  # TCP 服务器 + handler 注册
│       ├── CommandDispatcher.h/.cpp
│       ├── FreeRunController.h/.cpp
│       ├── RtTestController.h/.cpp
│       └── handlers/          # JSON-RPC handler
├── src/
│   ├── core/                  # 共享类型
│   └── igh/                   # IgH 适配
├── tests/                     # 111 个测试
├── translations/              # 8 个 .ts 翻译文件
├── themes/                    # 12 个 .qss 主题文件
├── docs/                      # 文档
│   ├── ARCHITECTURE.md        # 架构文档
│   ├── PLUGIN_GUIDE.md        # 插件开发指南
│   ├── DEVELOPMENT.md         # 开发指南
│   └── superpowers/           # 设计文档和计划
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
- ServiceContainer 持有所有 Service 实例 (33 个)，注入到 Plugin 构造函数

### 守护进程
- JSON-RPC over TCP (127.0.0.1:5877)
- CommandDispatcher 分发到 handler
- 直接 ecrt API (FreeRun) + CLI fallback (其他)

### 测试
- 111 个单元测试，覆盖模型层、适配器、插件、服务、集成和性能测试
- GUI 测试使用 `QT_QPA_PLATFORM=offscreen`
- TDD 驱动开发

---

## 6. 构建和测试命令

```bash
# 配置
cmake -B build

# 构建
cmake --build build -j4

# 运行所有测试
ctest --test-dir build --output-on-failure -j4

# GUI 冒烟测试
QT_QPA_PLATFORM=offscreen timeout 5s build/apps/ecat-studio/ecat-studio

# 推送到 GitHub (需要代理)
export all_proxy=http://127.0.0.1:7890
git push origin main
```

---

## 7. 已知问题

1. **无重大问题** — 所有 111 个测试通过
2. **性能优化空间** — 可以进一步优化缓存策略和异步操作
3. **文档完善** — 可以添加更多示例和教程

---

## 8. Git 状态

```
分支: main
远程: origin → https://github.com/NekoRain404/NekoEcat-Studio.git
```

---

## 9. 联系和上下文

- 用户语言：中文 (zh-CN)
- 开发偏好：TDD、功能优先测试 (~70% 覆盖)、激进重构策略
- 代码风格：2-space indent, `#pragma once`, 描述性注释
- 并发限制：最多 2 个子代理并行

**交接完成。祝 mimoCode 开发顺利。**
