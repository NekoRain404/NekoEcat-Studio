# NekoEcat Studio v2 — 综合设计文档

> **Date:** 2026-06-18
> **Status:** Approved
> **Scope:** 插件系统激活 + God Object 拆解 + 图形拓扑 + DC 配置 + ESI 仓库 + 总线统计 (1个月)

---

## 1. 目标

将 NekoEcat Studio 从「插件架构已定义但未激活」的状态，推进到「完全插件化 + 对标 TwinCAT 核心功能」的状态。

### 具体目标

1. 激活插件系统 — 所有工作区由 PluginRegistry 管理，MainWindow 变为 <500 行薄壳
2. 拆解 God Object — 24K 行工作区代码提取为 13+ 个独立 Plugin 类
3. 图形拓扑可视化 — QGraphicsScene 实现总线拓扑图，支持交互
4. DC 配置工程 UI — 参考时钟切换、同步周期配置、偏移补偿、时钟传播路径可视化
5. ESI 仓库管理 — 解析/生成/匹配 ESI XML 文件，自动填充 OD/PDO 描述
6. 总线统计仪表盘 — 帧计数、字节计数、CRC 错误、丢帧率、从站状态分布

---

## 2. 架构设计

### 2.1 插件系统激活

MainWindow 变为薄壳，只负责：
- MenuBar / ToolBar / StatusBar
- TabHost (QTabWidget，由 PluginRegistry 驱动)
- ServiceContainer (持有所有 Service 实例)
- wire() 连接 EcatClient → EventBus → Services

```
┌─────────────────────────────────────────────┐
│  MainWindow (薄壳 <500行)                     │
│  - MenuBar / ToolBar / StatusBar             │
│  - TabHost (由 PluginRegistry 驱动)           │
│  - ServiceContainer                           │
└──────────┬──────────────────────────────────┘
           │
    ┌──────┴──────┐
    │  EventBus   │ ← 12 种事件类型
    └──────┬──────┘
           │
  ┌────────┼────────┬──────────┬───────────┬──────────┐
  │        │        │          │           │          │
┌─┴──┐ ┌──┴──┐ ┌───┴──┐ ┌────┴──┐ ┌──────┴────┐ ┌───┴───┐
│OD  │ │Watch│ │FreeRun│ │DcSync │ │Topology   │ │Signal │
└────┘ └─────┘ └──────┘ └───────┘ └───────────┘ └───────┘
```

### 2.2 ServiceContainer

```cpp
class ServiceContainer : public QObject {
  Q_OBJECT
public:
  SdoService *sdo;
  WatchService *watch;
  TopologyService *topology;
  DcSyncService *dcSync;
  AlEventService *alEvent;
  SignalService *signal;
  FreeRunService *freeRun;    // 新增
  BusStatsService *busStats;  // 新增
  EsiService *esi;            // 新增
};
```

### 2.3 插件注册流程

MainWindow 构造时：
1. 创建 ServiceContainer 及所有 Service 实例
2. 创建所有 Plugin 实例，注入 ServiceContainer 引用
3. 调用 PluginRegistry::registerPlugin() 注册
4. TabHost 从 PluginRegistry 读取插件列表，按 defaultOrder() 排序后创建标签页

---

## 3. MainWindow 拆解计划

### 3.1 拆解清单

| 工作区文件 | 目标插件 | 行数 | 复杂度 |
|-----------|---------|------|--------|
| MainWindowUiBuild.cpp (总览部分) | OverviewPlugin | ~300 | 中 |
| MainWindowSdoWorkspace + Selection + Write + TargetPanel | OdPlugin | ~4500 | 高 |
| MainWindowWatchWorkspace.cpp | WatchPlugin | ~600 | 中 |
| MainWindowFreeRunChart.cpp | FreeRunPlugin | ~500 | 中 |
| MainWindowStartupSdoWorkspace.cpp | StartupSdoPlugin | 1098 | 高 |
| MainWindowIoVariableWorkspace.cpp | IoVariablePlugin | 835 | 中 |
| MainWindowConsistency.cpp | ConsistencyPlugin | 597 | 低 |
| MainWindowStateMachine.cpp | StateMachinePlugin | 675 | 中 |
| MainWindowDiagnosticsTopology.cpp | DiagnosticsPlugin | ~400 | 低 |
| MainWindowRtTestWorkspace.cpp | RtTestPlugin | 532 | 中 |
| MainWindowTopologyUi.cpp | TopologyPlugin (扩展) | 1134 | 高 |
| MainWindowSessionWorkspace.cpp | SessionPlugin | 1275 | 高 |
| MainWindowExport.cpp | ExportPlugin | ~300 | 低 |
| MainWindowContextMenus.cpp | 分散到各插件 | 1555 | 高 |
| MainWindowCommandPalette.cpp | MainWindow 保留 | 1922 | 高 |

### 3.2 拆解顺序（从简到难）

1. ConsistencyPlugin (597行, 低耦合)
2. StateMachinePlugin (675行, 中等耦合)
3. DiagnosticsPlugin (~400行, 低耦合)
4. RtTestPlugin (532行, 中等耦合)
5. WatchPlugin (~600行, 中等耦合)
6. FreeRunPlugin (~500行, 中等耦合)
7. IoVariablePlugin (835行, 中等耦合)
8. StartupSdoPlugin (1098行, 高耦合)
9. OdPlugin (4500行合计, 高耦合)
10. SessionPlugin (1275行, 高耦合)
11. OverviewPlugin (~300行, 但依赖其他插件)
12. ContextMenus → 分散到各插件
13. CommandPalette → 保留或提取为独立组件

### 3.3 状态迁移

MainWindow 当前持有的工作区状态迁移到 Service 或 Plugin：

| MainWindow 成员 | 迁移目标 |
|----------------|---------|
| `slaves_` | TopologyService |
| `sdo_->sdoTable` | OdPlugin 私有成员 |
| `watch_->watchTable` | WatchPlugin 私有成员 |
| `topologyBaseline_` | TopologyService |
| `consistencyFresh_` | ConsistencyPlugin |
| `loadedPdoPosition_` | TopologyService |
| `freeRunWidgets_` | FreeRunPlugin 私有成员 |

---

## 4. 图形拓扑可视化

### 4.1 技术方案：QGraphicsScene

每个从站是一个可交互的图形节点 (SlaveNodeItem : QGraphicsItem)。

### 4.2 节点设计

```
┌──────────────────────┐
│  ● EL1008            │  绿色=OP, 黄色=SAFEOP, 红色=INIT, 灰色=未扫描
│  Pos: 0              │
│  DC: ✓ Sync          │  DC 同步状态指示
│  IN: 8bit  OUT: 0bit │
└──────────────────────┘
```

### 4.3 布局算法

- **线性布局**（默认）：从站按物理顺序从左到右排列，模拟 DIN 导轨
- **树形布局**：如果有分支拓扑（Y 型连接器），按树形展开
- 支持鼠标拖拽重新排列

### 4.4 连线

- 主站节点在最左侧，从站依次向右
- 连线颜色：绿色=正常通信, 红色=链路断开, 虚线=Hot Connect

### 4.5 交互

- 鼠标滚轮缩放，中键拖拽平移
- 点击节点弹出详情面板
- 双击节点跳转到该从站的 OD/Watch 标签页
- 右键菜单：查看信息、切换状态、DC 配置

### 4.6 类设计

```cpp
class SlaveNodeItem : public QGraphicsItem {
  int position_;
  SlaveInfo info_;
  QRectF boundingRect() const override;
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
  void mousePressEvent(QGraphicsSceneMouseEvent*) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;
};

class TopologyGraphWidget : public QGraphicsView {
  Q_OBJECT
public:
  void updateTopology(const QVector<SlaveInfo> &slaves);
  void setLayoutMode(LayoutMode mode);
signals:
  void slaveSelected(int position);
  void slaveDoubleClicked(int position);
private:
  QGraphicsScene *scene_;
  QVector<SlaveNodeItem*> nodes_;
  QVector<QGraphicsLineItem*> links_;
};
```

---

## 5. DC 配置工程 UI

### 5.1 功能矩阵

| 功能 | 说明 |
|------|------|
| 参考时钟管理 | 查看/切换参考时钟从站 |
| 同步周期配置 | 设置 Sync0/Sync1 周期时间 |
| 偏移补偿 | 配置每个从站的 DC 偏移量 |
| 时钟传播路径 | 图形化展示 DC 时钟在总线上的传播链路 |
| 同步质量监控 | 漂移/抖动实时图表（增强 DcSyncPlugin） |

### 5.2 UI 布局

左面板：从站列表（带 DC 状态指示）
右面板：配置表单 + 时钟传播路径图 + 同步质量趋势图

### 5.3 Daemon 扩展

- `dcConfigure` — 写入 DC 配置参数（ecrt_slave_config_dc）
- `dcRefClockSet` — 切换参考时钟从站（ecrt_master_reference_clock_time）

### 5.4 类设计

```cpp
class DcConfigPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  QString id() const override { return "dcconfig"; }
  QString displayName() const override { return "DC Config"; }
  QString displayNameZh() const override { return "DC 配置"; }
  int defaultOrder() const override { return 62; }
  QWidget *widget() override;
private:
  QSplitter *splitter_;
  QTableWidget *slaveList_;
  QComboBox *refClockCombo_;
  QSpinBox *syncCycleSpin_;
  QSpinBox *sync0OffsetSpin_;
  QSpinBox *sync1OffsetSpin_;
  TopologyGraphWidget *clockPathView_;
  DcSyncService *dcSyncService_;
};
```

---

## 6. ESI 仓库管理

### 6.1 功能

| 功能 | 说明 |
|------|------|
| ESI 文件导入 | 导入 XML 格式 ESI 文件到本地仓库 |
| ESI 文件解析 | 解析设备名称、Vendor/Product、PDO 映射、SDO 字典 |
| 自动匹配 | 扫描在线从站时自动匹配 Vendor ID + Product Code |
| PDO/SDO 填充 | 匹配成功后自动填充描述信息 |
| ESI 导出 | 从在线从站信息生成 ESI XML |
| 仓库浏览 | 按厂商/产品分类浏览 |

### 6.2 Daemon 扩展

- `esiImport` — 读取 ESI 文件，存入 ~/.config/NekoEcat/esi/
- `esiMatch` — 给定 Vendor ID + Product Code，返回匹配的 ESI 信息
- `esiExport` — 从在线从站信息生成 ESI XML
- `esiList` — 列出仓库中所有 ESI 条目

### 6.3 自动匹配工作流

```
TopologyService::scan() 完成
  → 对每个从站调用 EsiService::matchDevice(vendorId, productCode)
    → 匹配成功: 自动填充 OD 描述、PDO 映射名称
    → 匹配失败: 在 Overview 显示 "ESI missing" 警告
```

### 6.4 类设计

```cpp
class EsiHandler {
public:
  QJsonObject handleImport(const QString &id, const QJsonObject &params);
  QJsonObject handleMatch(const QString &id, const QJsonObject &params);
  QJsonObject handleExport(const QString &id, const QJsonObject &params);
  QJsonObject handleList(const QString &id, const QJsonObject &params);
private:
  QString repoPath_;
  struct EsiEntry {
    QString filePath, vendorName, deviceName;
    uint32_t vendorId, productCode;
    QStringList pdoNames;
  };
  QVector<EsiEntry> entries_;
  void scanRepository();
};

class EsiService : public QObject {
  Q_OBJECT
public:
  void importFile(const QString &path);
  void matchDevice(uint32_t vendorId, uint32_t productCode);
  void exportCurrent(int position);
  void refreshList();
signals:
  void esiListUpdated(const QJsonArray &entries);
  void matchResult(const QJsonObject &esiInfo);
  void error(const QString &msg);
};

class EsiPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  QString id() const override { return "esi"; }
  QString displayName() const override { return "ESI Repository"; }
  QString displayNameZh() const override { return "ESI 仓库"; }
  int defaultOrder() const override { return 80; }
  QWidget *widget() override;
};
```

---

## 7. 总线统计仪表盘

### 7.1 指标

| 指标 | 来源 |
|------|------|
| Tx/Rx 帧计数 | `ethercat master` 输出 |
| Tx/Rx 字节数 | `ethercat master` 输出 |
| CRC 错误 | IgH sysfs |
| 丢帧率 | 计算得出 |
| 主站状态 | `ethercat master` |
| 从站状态分布 | `ethercat slaves` |

### 7.2 Daemon 扩展

- `busStats` handler — 聚合 master 状态、帧计数、从站状态分布

### 7.3 类设计

```cpp
class BusStatsPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  QString id() const override { return "busstats"; }
  QString displayName() const override { return "Bus Stats"; }
  QString displayNameZh() const override { return "总线统计"; }
  int defaultOrder() const override { return 63; }
  QWidget *widget() override;
};
```

---

## 8. 测试策略

- 功能优先，覆盖率 ~70%
- 每个新 Plugin 有身份测试 (id, displayName, defaultOrder, visible)
- 每个新 Service 有信号转发测试
- 每个新 Daemon Handler 有 JSON 结构测试
- 集成测试：daemon 生命周期、GUI 冒烟
- 目标：100+ 测试

---

## 9. 交付计划

| 周 | 重点 | 交付物 |
|----|------|--------|
| W1 | 插件系统激活 + 简单工作区迁移 | ServiceContainer, 5 个 Plugin 迁移 |
| W2 | 剩余工作区迁移 + MainWindow 瘦身 | 所有 Plugin 迁移完成, MainWindow <500 行 |
| W3 | 图形拓扑 + DC 配置 + ESI 仓库 | 3 个新功能 Plugin + Daemon Handler |
| W4 | 总线统计 + 测试 + CI/CD + 文档 + 发布 | v2.0.0 Release |

---

## 10. 风险与缓解

| 风险 | 影响 | 缓解 |
|------|------|------|
| MainWindow 状态依赖复杂 | 拆解时遗漏状态迁移 | 逐个工作区迁移，每步运行全部测试 |
| QGraphicsScene 性能 | 大量从站时拓扑图卡顿 | 限制最大节点数，使用缓存渲染 |
| IgH DC API 有限 | dcConfigure 可能不支持所有参数 | 检测 API 可用性，不可用时退化为只读 |
| ESI XML 格式复杂 | 解析器可能不兼容所有厂商 | 先支持标准格式，异常时降级为基本信息 |
