# NekoEcat Studio 项目概览

## 项目定位

NekoEcat Studio 是一个基于 IgH EtherCAT Master 的现代化 EtherCAT 主站工程软件，目标是形成类似 TwinCAT/System Manager 的桌面工程环境。

当前项目不是单纯 UI，而是已经拆分为 GUI、运行时 daemon、IgH 适配层和共享协议层。IgH 主站作为底层依赖，NekoEcat Studio 负责工程配置、在线诊断、Free Run、SDO/PDO 查看、启动参数管理和更现代的用户界面。

当前目标：

- 使用已安装的 IgH EtherCAT Master 作为底层主站依赖
- 提供现代化 Qt 桌面 GUI
- 提供本地 runtime daemon：`ecatd`
- 支持扫描从站、查看主站状态、PDO/SDO、ESI XML、状态切换、SDO 读写
- 支持 Free Run，并单独显示实时回报内容
- 支持 I/O Variables 工程信号表，把 PDO、Free Run、Watch 和 Startup SDO 证据合并到同一审阅面
- 支持 Consistency 一致性页，用只读方式对比拓扑基线、Startup、Watch、I/O Variables 和工程元数据
- 支持状态机视图，以从站状态、证据、风险和保守推荐状态辅助 PREOP/SAFEOP/OP 推进
- 支持工程文件保存、启动 SDO、诊断、备注、设置等成熟软件基础功能

## 技术架构

当前项目使用：

- C++20
- Qt6 Widgets
- Qt6 Network
- CMake
- IgH EtherCAT Master
- 本地 TCP JSON 协议

整体架构：

```text
ecat-studio GUI
    |
    | 127.0.0.1:5877
    | newline-delimited JSON
    v
ecatd runtime daemon
    |
    +--> IgH ethercat CLI
    +--> ecrt Free Run runtime
```

这种拆分方式使 GUI 不直接依赖 IgH 命令调用细节，也便于后续把部分 CLI 功能替换为原生 `ecrt.h` runtime 能力。

## 目录结构

```text
.
├── CMakeLists.txt
├── apps
│   ├── ecat-studio
│   │   ├── EcatClient.cpp
│   │   ├── EcatClient.h
│   │   ├── MainWindow.cpp
│   │   ├── MainWindow.h
│   │   ├── SettingsDialog.cpp
│   │   ├── SettingsDialog.h
│   │   └── main.cpp
│   └── ecatd
│       ├── EcatDaemon.cpp
│       ├── EcatDaemon.h
│       ├── FreeRunController.cpp
│       ├── FreeRunController.h
│       └── main.cpp
├── docs
│   ├── DEVELOPMENT.md
│   └── PROJECT_OVERVIEW.md
└── src
    ├── core
    │   ├── EthercatTypes.cpp
    │   ├── EthercatTypes.h
    │   ├── JsonProtocol.cpp
    │   └── JsonProtocol.h
    └── igh
        ├── EthercatCliBackend.cpp
        └── EthercatCliBackend.h
```

## 核心模块

### ecat-studio

`apps/ecat-studio` 是 Qt GUI 主程序。

主要职责：

- 构建主窗口和全部工程界面
- 管理工程文件打开、保存、另存为
- 显示主站、从站、PDO、SDO、ESI XML、诊断和 Free Run 数据
- 管理设置项，包括主题、语言、缩放和多主站配置
- 通过 `EcatClient` 与 `ecatd` 通信

当前 GUI 已包含：

- 工程菜单：新建、打开、保存、另存为
- 工具菜单：连接 runtime、刷新、重新扫描、导入 ESI XML、设置
- 帮助菜单：使用说明书、关于软件、日志窗口
- 主站选择器
- 拓扑树
- 总览页，包含只读 Session Brief / 会话简报、从站证据矩阵、按阶段/风险/证据/动作组织的调试工作流看板和下一最佳动作
- Free Run 页
- I/O Variables 页，合并 PDO Map、Free Run 过程映像、Watch 值、Startup 期望、映射状态和变化证据
- Consistency 一致性页，只读审阅 Online/Offline 证据、拓扑基线、Startup 偏差、I/O 变量缺口和工程元数据
- Watch 表，支持当前从站、变化项、基线偏离、Startup 不一致、缺失值和 CiA 402 范围过滤
- Startup SDO 页，支持选中行本地详情条、Watch Value/Watch Delta 证据和只看偏差审阅
- State Machine 状态机页
- Notes 页
- ESI Repository 页
- Diagnostics 页
- PDO Map 页
- Object Dictionary 页
- ESI XML 原始视图
- Master/Slave/PDO/SDO 原始输出视图

### ecatd

`apps/ecatd` 是本地 runtime daemon。

主要职责：

- 监听 `127.0.0.1:5877`
- 接收 GUI 发来的 JSON 请求
- 调用 IgH 后端或 Free Run runtime
- 返回结构化结果或错误信息

当前 daemon 支持的方法：

- `ping`
- `master`
- `scan`
- `rescan`
- `slaveInfo`
- `pdos`
- `sdos`
- `xml`
- `upload`
- `download`
- `applyStartupSdos`
- `setState`
- `setAllStates`
- `freeRunStart`
- `freeRunStop`
- `freeRunStatus`

### src/igh

`src/igh` 是 IgH EtherCAT Master 的适配层。

当前主要通过已安装的 `ethercat` CLI 实现：

- `ethercat master`
- `ethercat slaves`
- `ethercat rescan`
- `ethercat states`
- `ethercat pdos`
- `ethercat sdos`
- `ethercat upload`
- `ethercat download`
- `ethercat xml`

后续可以在该层继续扩展原生 `ecrt.h` 后端，减少对 CLI 文本解析的依赖。

### src/core

`src/core` 是共享核心模块。

主要包含：

- EtherCAT 数据类型
- 从站信息结构
- JSON 请求/响应协议
- GUI 和 daemon 共享的数据转换逻辑

## 当前已实现功能

### 在线主站功能

- 查看 IgH master 状态
- 扫描从站
- 重新扫描总线
- 查看从站信息
- 查看从站 PDO
- 查看从站 SDO 字典
- 查看从站 XML
- 设置单个从站状态
- 设置全部从站状态

### 状态机功能

State Machine 页当前已经具备独立工作区：

- 按从站展示当前 EtherCAT 状态
- 汇总身份、OD、Watch、CiA 402 驱动、Startup SDO、PDO 和 Free Run 过程证据
- 标出 Startup 偏差、PDO 映射问题、拓扑基线问题和证据缺失风险
- 选中行会刷新状态切换详情条，显示当前状态、推荐状态、证据、驱动、Startup、PDO/过程证据、风险和确认边界；选择和预览只使用本地表格证据，不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health
- 采用保守推荐逻辑：INIT 推荐 PREOP；PREOP 在 PDO 和 Watch 证据具备后推荐 SAFEOP；SAFEOP 在过程映像证据和 Startup/PDO 风险清理后推荐 OP
- 支持双击行或按钮发送推荐状态
- 支持行级 PREOP / SAFEOP / OP 请求
- 支持全部 PREOP / 全部 SAFEOP 请求
- 所有状态请求仍复用原有确认流程，不直接绕过安全确认

### SDO 功能

- SDO upload
- SDO download
- 支持类型选择
- Object Dictionary 的 Selected Object 目标工作台会在选中对象或从 Watch/Startup/PDO/I/O 回填对象后，用当前 slave/index/subindex 聚合同一对象的 OD 证据、Watch 行、Startup SDO 行、Object Bookmark、SDO Target Trail、写入权限、校验状态、证据集一致性、写入差异和下一步建议；Evidence Set 会在写入框为空时也显示本地证据是否一致；Use Evidence 会把最佳本地 Read/Watch/OD/Startup/Bookmark/Target Trail 值回填到写入框，Pick Evidence 会列出同一组本地候选供手动选择；Write Delta 会在写入确认前只用本地 Read/OD/Watch/Startup/Bookmark/Target Trail 证据比较待写值是否匹配、不同或证据冲突；Copy Evidence / 复制证据会把当前目标、Selected Object 复核行、本地证据候选、Watch/Startup/书签/目标轨迹链接和本地边界复制到剪贴板，便于交接或写入前复核；Selected Object 表新增 Action / 动作列，直接显示每一行的本地意图，例如 Open Watch、Open Startup、Open Trail、Review Delta、Focus OD 或 Copy Digest；Run Row Action / 执行本行动作按钮会随当前行动态显示为 Run: Open Watch / 执行：打开 Watch 等具体动作，并执行当前行 Action 列对应的本地路由，减少用户记忆双击、快捷键或右键入口的负担；Copy Row / 复制本行按钮会随当前行动态显示为 Copy: Target / 复制：目标等具体行证据复制入口，只写剪贴板；Selected Object 行可双击或按 Alt+Enter 本地路由，Watch/Startup/书签/目标轨迹行会打开匹配证据，Evidence Set / Write Delta 行会审阅冲突或差异，Target/Read/OD 行会聚焦对象字典上下文，摘要行会复制证据摘要；Selected Object 右键菜单提供 Open Row Evidence / Copy Row Evidence / Copy Full Evidence Digest，把行级证据动作从隐藏快捷键变成显式入口；命令面板也提供 Open Selected Object Row Evidence 和 Copy Selected Object Row Evidence；SDO 写入确认会针对即将写入的同一 slave/index/subindex 再汇总 Read/Watch/OD/Startup/Bookmark/Target Trail Evidence Set，并标明目标值匹配全部证据、只匹配部分证据或不匹配本地证据；危险确认弹窗会把这些影响按 Critical Impact / Review Before Confirming / Evidence / Target Context（关键影响、确认前复核、证据、目标上下文）分组，先显示驱动/输出/持久化/拓扑/一致性/缺失证据等高影响项；Review Delta 在写入框为空时审阅 Evidence Set 冲突，在有待写值时优先打开造成写入差异的 Watch/Startup/Bookmark/Target Trail/OD 本地证据行；SDO Target Trail 会把来自 Object Dictionary、PDO Map、Watch、Free Run、I/O Variables、SDO History、Startup SDO、Object Bookmarks、CiA 402 辅助动作和手动字段的最近本地目标保存到工程，选中行会显示 Time、Slave、Index/Sub、Type、Source、Value、Write、Detail、复用就绪度和边界；双击或 Restore Target 可恢复目标，也可把所选轨迹行本地加入 Watch、保存为 Object Bookmark，或用轨迹写入值/最后值生成 Startup SDO 候选；匹配轨迹行会参与 Selected Object 的 Evidence Set、Use Evidence、Pick Evidence 和 Write Delta 审阅，并可通过 Open Trail / 打开轨迹定位；面板、目标轨迹和命令面板都支持打开当前对象匹配的 Watch/Startup/书签/目标轨迹证据行；这些入口只整理本地上下文、复制证据摘要和跳转本地证据，不主动读总线、不写 SDO、不切状态、不启动 Free Run，也不运行 Host Health
- 对象字典支持工程内 Object Bookmarks，用于收藏常用 SDO 对象、快速回填 SDO 字段、加入 Watch、用保存的 Last Value 生成 Startup SDO 候选，并保存到 `.ecatproj`；选中书签会显示目标、权限、类型、位宽、名称、Last Value、Source、复用就绪度和本地边界；选择、详情预览、回填、加入 Watch、移除和用保存值生成 Startup SDO 候选都不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health
- 对象字典可把所选且已有 Last Value 的 OD 证据直接生成或更新 Startup SDO 候选，不需要先收藏，也不会读写总线
- SDO History 选中行会刷新本地审计详情条，显示 Time、Action、Slave、Index、Sub、Type、Value、Status、Detail、复用就绪度和操作边界；选择历史行、查看详情、从历史回填 SDO 字段、把历史行加入 Watch，以及用已完成且带值的历史行创建 Startup SDO 候选都只整理本地证据，不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health；双击或 Fill and Read 是显式 SDO 读取路径，后续 Refresh Watch 才读取 Watch 行，Startup Apply 仍在确认后写入 Startup 行
- Watch 表记录监视值、基线偏离、Startup 期望对比和范围过滤视图
- Startup SDO 批量配置，选中行会显示目标、期望值、状态、详情、Watch Value、Watch Delta 和写入边界；选择行、只看偏差和详情预览只使用本地表格证据
- Startup SDO 批量应用，Verify/校验会读取目标 SDO；Apply Row、Apply Selected、Apply Diffs 和 Apply Startup 只在确认后写入 SDO 值

### PDO 功能

- 读取 IgH PDO 文本输出
- 解析 PDO 映射为表格
- 显示 SM、PDO、Index、SubIndex、Bits、Name
- 选中行会刷新本地详情条，显示 Sync Manager、PDO、方向、Index/Sub、位宽、推断 SDO 类型、名称、过程角色、CiA 402 候选证据和操作边界
- PDO Map 加载后，选择行、筛选和详情预览只使用本地表格证据；加载/刷新 PDO Map 是显式在线 PDO 证据路径，双击行会通过普通 SDO 读取路径读取对象，Add Selected to Watch 只创建 Watch 行直到刷新
- 保留 PDO raw 输出，便于调试对照

### Free Run 功能

Free Run 已经具备独立页面，不只是简单开关。

当前 Free Run 页显示：

- Running / Stopped 状态
- Domain state
- Working counter
- WKC state
- AL state
- PDO entry 数量
- 实时 PDO entry 表格

PDO entry 当前拆分显示：

- `Name`
- `Raw`
- `Decoded`
- `Meaning`
- `Map Status` / `Map Detail`，其中 `Map Detail` 会显示 Free Run 条目名称来源

这样可以避免把原始值、解码值和含义混在同一个单元格中，调试时更清楚。Free Run 的 `Name` 会优先使用工程内 I/O Alias，其次使用运行时显示名、运行时名称、PDO Map 名称、对象级缓存、条目级缓存，最后回退到对象地址；运行时偶发丢失名称字段时，界面仍能保留稳定可读的信号名，并能在 `Map Detail` 中看到名称来源。

选中 Free Run 过程映像行时，页面会刷新本地详情条，直接显示从站、方向、对象地址、Name、Name 来源、Raw/Decoded/Meaning、位位置、PDO 映射状态、是否变化和输入/输出边界。这个详情条只读当前表格证据；选择、过滤和详情预览都不读取 SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。显式切换 Free Run 仍然走原有启动确认流程，`Fill SDO Fields` / `Alt+Enter` 只本地回填 SDO 目标，除非用户选择 `Fill and Read SDO`。

### I/O 变量功能

I/O Variables 页已经具备工程信号表能力：

- 合并 Free Run 过程映像、已加载 PDO Map、Watch 值和 Startup SDO 期望
- 显示 Slave、方向、Symbol、Index、Sub、Bits、PDO、Source、Raw、Decoded、Meaning、Watch、Startup、Map、Changed、PLC、Alias、Tags、Note
- 支持范围过滤：全部、当前从站、过程映像、仅 PDO、Watch 证据、Startup 不一致、缺失值、Rx 输出、Tx 输入、CiA 402、变化项、PLC 交接问题
- 支持筛选文本快速定位名称、Index、Sub、方向、值、含义、映射状态、Alias、Tags 或 Note
- 选中行会刷新本地详情条，显示信号名/Alias、对象地址、方向、来源、Raw/Decoded/Meaning、Watch 值、Startup 对照、PDO 映射状态、变化标记、PLC 质量、Tags、Note 和操作边界
- 支持从所选或可见 I/O 变量证据生成/更新 Startup SDO 候选，优先使用 Watch 值，Raw 值兜底，且不读写总线
- 选中或双击变量可自动回填 SDO 指令区，显式 Read SDO 时才发起读取
- 支持把所选或可见变量加入 Watch，复用已有 Watch 行，不立即进行大量 mailbox 读取
- 支持工程内 Alias、Tags、Note 元数据，按从站 position、Index、SubIndex 绑定，保存到 `.ecatproj`
- 支持导出当前可见 I/O Variables CSV，用于现场交接、调试记录和后续 PLC 变量规划
- 支持对所选或可见 I/O 变量批量生成工程 Alias/Tags，默认保护已有 Alias，覆盖已有 Alias 前会二次确认；该动作只修改工程元数据，不访问总线
- 支持把所选或当前可见 I/O 变量复制为 IEC 风格 `VAR_GLOBAL` PLC 声明块，复用标准化 Symbol、IEC Type、对象地址、方向和 PLC Quality 证据，复制前会运行同一 PLC 交接质量提示，方便现场直接粘贴到 PLC 工程且保留问题证据
- 支持导出当前可见 I/O 变量的 PLC Symbols CSV，包含标准化 Symbol、Direction、IEC 风格 Type、Slave、Index、SubIndex、PDO、证据值、PLC Quality、Alias、Tags、Note 和对象地址
- 支持把当前可见 I/O 变量导出为 `.st` IEC `VAR_GLOBAL` 声明文件，复用复制声明的生成器和同一 PLC 交接质量门禁
- PLC 质量列会标出 Missing Alias、Auto Name、No Tags、Duplicate Symbol 等交接问题，并纳入一致性检查提示
- 支持通过 I/O 页按钮、工程菜单、命令面板和右键菜单直接审阅 PLC 交接问题；保存 PLC Symbols CSV、导出 PLC 声明 ST 或复制 PLC 声明前会检查当前交接范围，提示继续输出、取消或跳转审阅问题，且该检查只使用工程表格证据，不读写总线、不触发主机诊断
- 高亮 Startup 偏差、过程值变化、映射缺失、方向/位宽不一致和 CiA 402 关键对象
- 导出诊断报告时会写出当前 I/O Variables 表，方便现场交接和后续复盘

这个功能已经覆盖“真实 I/O 变量表”的第一阶段：从在线 PDO、Free Run、Watch 和 Startup 证据建立可操作信号表，并支持工程内别名、标签、备注、批量命名、聚焦 PLC 交接质量复核、带质量门禁的 IEC 风格 PLC 声明复制/导出、导出前质量提示、普通 CSV 导出和 PLC 符号规划 CSV 导出。后续仍需要设备库驱动的离线符号绑定、更多 PLC 厂商格式模板和更完整的 Online/Offline 变量对比。

### 一致性 / Online-Offline 对比功能

Consistency 页当前已经具备第一阶段 Online/Offline 工程审阅能力：

- 只使用当前工程和会话中已有证据，不主动读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行主机诊断
- 作为 Commissioning Workflow 和 Next Best Action 的只读一致性门禁，排在 Startup/Watch 偏差审阅之后、Free Run 过程映像验证和后续状态推进之前；它只提供工程证据判断，不承担主机诊断
- Watch、Startup、PDO、Free Run、拓扑或 I/O 变量元数据变化后，一致性结果会视为过期，需要重新刷新才算通过
- Free Run 启动确认、单从站/全部从站状态切换确认会显示一致性门禁状态；状态机从 SAFEOP 推荐 OP 时也要求一致性门禁通过
- SDO 写入、Startup SDO 应用、Free Run 启动、单从站/全部从站状态切换共用统一的风险分组确认审阅，把关键影响、确认前复核、证据和目标上下文分区展示；这只改变确认前的可读性和审计，不绕过任何既有校验、确认按钮或运行时请求路径
- 选中行会刷新本地详情条，显示级别、范围、目标、证据、期望/实际值、建议动作、最佳证据路由和只读门禁边界
- 对比拓扑基线和当前在线从站，发现缺失从站、额外从站、名称变化和状态变化
- 对比 Startup SDO 与 Watch / I/O Variables 证据，发现启动期望和当前证据不一致、缺少 Watch 对比、无效 Startup 行
- 审阅 I/O Variables 的过程值、PDO/过程映像映射状态、缺失值、PLC 交接质量、缺失工程 Alias，以及工程内 Alias/Tags/Note 元数据是否已经没有在线证据
- 一致性表可以通过 Open Evidence、双击、右键或命令面板打开最相关的本地证据表：拓扑行定位状态机，Startup 行定位 Startup SDO，Watch 证据定位 Watch 行，其余 I/O 问题跳回 I/O 变量页并自动选择 PLC 交接问题、Startup 不一致、缺失值或 PDO 证据等相关范围；全程不访问总线
- 当一致性门禁已经发现 Error/Warning 时，Next Best Action 可以直达第一条阻塞行的证据目标（状态机、Startup SDO、Watch 或 I/O 变量），而不是只打开汇总表
- Next Best Action 状态栏按钮使用语义颜色区分普通下一步、证据审阅、诊断错误、就绪和命令面板状态，降低高风险操作前的视觉歧义
- Overview 的 Session Brief / 会话简报会只用已加载界面证据汇总当前目标、一致性门禁、OD/PDO 映射、当前 SDO 本地证据组与待写值差异、Watch/Startup/Free Run 运行证据和下一步动作；Copy Row / 复制本行、右键 Copy Row Evidence / 复制本行证据 和命令面板 Copy Session Brief Row Evidence / 复制会话简报本行证据 会把当前行的状态、依据、下一步、本地路由、当前 Next Best Action、行提示详情和本地边界复制到剪贴板，便于交接；双击、按 Enter、右键 Open Local Evidence / 打开本地证据，或在命令面板执行 Open Session Brief Evidence / 打开会话简报证据，只会打开对应本地证据界面，例如拓扑树、一致性表、对象字典、Watch、Startup SDO、Free Run 或下一条工作流行；行复制和本地证据导航都不主动读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health
- Overview 的 Slave Evidence Matrix / 从站证据矩阵会按从站横向显示 Priority、状态、OD/PDO、Watch 有值、Startup 偏差、过程映像、PDO 映射风险和下一步证据动作；矩阵自动形成 P0 Fault / P1 Risk / P2 Action / P3 Ready 的调试优先级队列，优先展示故障、风险和待执行项；Overview 选项卡徽标、边界提示、矩阵摘要和 P0/P1/P2/P3 快速 triage 按钮会显示实时优先级数量，强优先级在线前置条件处理完后，Next Best Action / 下一最佳动作也可以直接路由到优先级最高的矩阵问题；支持通过快速 triage 按钮或范围下拉框按 All/P0 Fault/P1 Risk/P2 Action/P3 Ready/Risk/Action/Ready/Missing OD/Missing PDO/Missing Watch/Startup Diff/Process Missing 过滤，并可搜索从站、优先级、状态、风险或下一步动作，适合大型总线快速定位证据缺口；Review First / 审阅首个问题 会打开当前可见列表里优先级最高的风险或待执行行，Review Next / 审阅下个问题 会从当前矩阵行继续打开下一个可见问题并在末尾回绕，让过滤后的矩阵成为可连续处理的问题清单；Copy Row / 复制本行、右键 Copy Matrix Row Evidence / 复制矩阵本行证据 和命令面板 Copy Slave Matrix Row Evidence / 复制从站矩阵本行证据 会把当前行的优先级、就绪度、风险、下一步、详细 tooltip 证据、当前过滤范围和本地边界复制到剪贴板，便于现场交接或问题报告；双击、`Alt+Enter`、右键 Open Matrix Evidence / 打开矩阵证据、Review First Matrix Issue / 审阅首个矩阵问题、Review Next Matrix Issue / 审阅下个矩阵问题 或 Copy Matrix Row Evidence / 复制矩阵本行证据，或命令面板 Open Slave Matrix Evidence / 打开从站矩阵证据、Review First Slave Matrix Issue / 审阅首个从站矩阵问题、Review Next Slave Matrix Issue / 审阅下个从站矩阵问题 或 Copy Slave Matrix Row Evidence / 复制从站矩阵本行证据，会本地选中从站、路由到最相关的已加载证据表，或复制本行摘要，例如 Object Dictionary、PDO Map、Watch、Startup SDO、Free Run 或 State Machine；过滤、导航和本行复制都不读取总线、不加载 OD/PDO/ESI、不切换状态、不改变 Free Run，也不运行 Host Health
- Overview 的调试工作流现在是面向工程调试的流程看板，每行显示 Phase/Status/Step/Risk/Evidence/Next Action，用于在执行写入、Free Run 或状态推进前直接暴露拓扑、OD、PDO、Watch、Startup、一致性和过程映像证据缺口；支持按 All/Open/Blocked/Action/Ready/Risk/Evidence Gap 范围过滤，并可搜索阶段、状态、步骤、风险、依据或下一步动作，命令面板也可以切换同一组 Workflow Scope，用于键盘驱动复核；选中某一行会刷新详情条，显示行号、阶段、状态、动作边界、风险、依据和下一步，并在 tooltip 中说明显式执行会保持本地审阅、连接/重扫在线、加载 OD/PDO、加入本地 Watch 行、打开一致性证据或切换 Free Run 遥测；Review First / 审阅首个 会选择当前可见首个未就绪工作流问题，Review Next / 审阅下个 会继续选择下一个可见问题并在末尾回绕，右键菜单和命令面板也提供同样入口；Copy Step / 复制步骤、右键 Copy Step Evidence / 复制步骤证据、`Alt+Enter` 和命令面板 Copy Workflow Step Evidence / 复制工作流步骤证据 会把当前步骤的阶段、状态、风险、依据、下一步、就绪度摘要、tooltip 详情、当前 Next Best Action 和本地边界复制到剪贴板，便于交接或执行前复核；Run Next 和双击行仍只执行该行已有的显式动作，工作流选择、过滤、问题审阅、详情复核和步骤证据复制不会读取总线、不加载 OD/PDO/ESI、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health
- 高频页签会显示本地证据/风险徽标，例如 Watch 行数、Startup Watch 偏差、I/O 问题、一致性 Error/Warning、状态机风险和 Diagnostics Error/Warning；徽标只使用已加载表格，不触发总线或主机检查
- Watch 页选中行会刷新本地详情条，直接显示当前值、解析含义、类型/模式、基线偏差、Startup 对照、是否变化和 CiA 402 候选状态；选择、范围筛选和详情预览都不读取 SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health；Refresh Watch 和 Auto 轮询才是显式 SDO 读取路径，Create Startup / Sync Startup 只改 Startup 表直到应用动作执行
- SDO History 页选中行会刷新本地审计详情条，显示操作时间、动作、从站、Index/Sub、类型、值、状态、详情、复用就绪度和边界提示；选择行、详情预览、从历史回填 SDO 字段、Watch Selected 和 Create Startup 只使用历史表证据，不读取 SDO、不写 SDO、不切换状态、不改变 Free Run、不重扫/连接，也不运行 Host Health；双击或 Fill and Read 是显式 SDO 读取路径，Refresh Watch 稍后才读取 Watch 行，Startup Apply 稍后才按普通确认流程写入 Startup 行
- PDO Map 页选中行会刷新本地详情条，显示 Sync Manager、PDO、方向、Index/Sub、位宽、推断类型、名称、过程角色、CiA 402 候选证据和操作边界；PDO Map 已加载后，选择行、筛选和详情预览不读取 SDO、不写 SDO、不切换状态、不改变 Free Run、不重扫/连接，也不运行 Host Health；加载/刷新 PDO Map 是显式在线 PDO 证据路径，双击行是显式 SDO 读取路径，Add Selected to Watch 只创建 Watch 行直到刷新
- Startup SDO 页选中行会刷新本地详情条，显示目标从站、Index/Sub、类型、期望值、状态、Detail、Watch Value、Watch Delta、证据等级和写入边界；选择行、只看偏差、行详情和偏差聚焦只整理本地表格证据，不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health；Verify Startup / Verify Selected 是显式 SDO 读取路径，Apply Row / Apply Selected / Apply Diffs / Apply Startup 是确认后的 SDO 写入路径
- I/O Variables 页选中行会刷新本地详情条，显示信号、对象地址、来源、Raw/Decoded/Meaning、Watch、Startup、Map、Changed、PLC、Alias/Tags/Note 和当前证据状态；选择行、切换范围、筛选、详情预览、复制 PLC 声明、导出可见行和编辑元数据都不读取 SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health；Fill SDO 只准备本地目标，Read SDO 和双击是显式 SDO 读取路径，Watch Selected/Visible 只创建 Watch 行直到刷新，Startup Selected/Visible 只改 Startup 表直到应用动作执行
- Consistency 页选中行会刷新本地详情条，显示级别、范围、目标、证据、期望、实际、建议动作和打开证据路由；选择行、筛选范围、详情预览、Open Evidence、双击行、右键证据导航和命令面板证据导航都只路由到已加载本地证据表，不读取 SDO、不写 SDO、不切换状态、不改变 Free Run、不重扫/连接，也不运行 Host Health；Refresh Check 只用当前 UI/工程证据重建一致性表
- 高频工作区支持稳定的纯导航入口：View / Workspaces 菜单、`Ctrl+Alt+1..0` 快捷键、`Alt+Left` / `Alt+Right` 工作区历史，以及命令面板中的 `Go to Workspace / 切换工作区`；这些入口只切换页签，不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health；历史和直达导航按实际页面定位，用户拖动重排页签后仍然有效
- 状态栏新增 Workspace Boundary / 工作区边界胶囊，随当前页签显示 Local Gate、Engineering、Online PDO、Watch Reads、SDO、Process Data、Startup Danger、State Danger、Host、File/ESI、Project 或 Raw Evidence 等边界；它只是常驻安全提示，不是动作按钮，悬停后说明当前页面哪些操作只处理本地证据、哪些会访问在线 SDO/PDO/过程数据、哪些保留危险确认流程
- 对象和证据表支持 `Alt+Enter` 本地证据动作：调试工作流行会复制步骤证据，会话简报和一致性行会打开本地证据，会话简报 Copy Row / 右键 / 命令面板可复制当前决策行摘要，从站证据矩阵会路由到最相关的已加载证据表，矩阵 Copy Row / 右键 / 命令面板可复制当前从站证据摘要，Selected Object 行会按 Action / 动作列提示本地打开证据、审阅差异、聚焦 OD 或复制证据摘要，Run Row Action 按钮、Copy Row 按钮、Selected Object 右键菜单也可以显式打开本行证据、复制本行证据或复制完整证据摘要，Object Dictionary、PDO Map、Watch、Free Run 条目、I/O Variables、SDO History、Startup SDO、Object Bookmarks 和 SDO Target Trail 行会回填当前 SDO 目标；该快捷键、工作流步骤复制、会话简报行复制、矩阵行复制、Run Row Action、Copy Row 和 Selected Object 行级右键动作都不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health，跨从站回填时只更新本地选中上下文和摘要
- 命令面板现在会给每条命令标注 Local / Online / Danger / Host / File（本地、在线、危险、主机、文件），并支持按操作类型过滤；支持直接执行调试下一步、切换工作流范围、审阅首个/下个工作流问题、复制工作流步骤证据、打开或复制会话简报证据、打开或复制从站矩阵证据等总览动作；在面板内可用 `Alt+A/L/O/D/H/F` 从键盘切换全部、本地、在线、危险、主机和文件过滤；用 `Alt+P` 或行右键菜单可以把高频命令固定到最近命令之前；固定命令和最近命令都只改变排序，不会自动运行命令；本地导航和工程表格整理会与运行时读取、主机检查、SDO 写入、状态切换、导入导出明显区分；结果摘要会显示当前搜索/过滤下的可执行命令数、固定命令数、最近命令数和各操作类型数量；当前选中命令会在固定预览区显示操作类型、安全边界、命令说明、固定/最近标记和当前上下文是否可执行；从命令面板显式触发命令会写入 Diagnostics / Event Stream，危险命令触发记录为 Warning，真正写入或切状态仍继续走已有确认流程
- 支持按 All、Errors、Warnings、Topology、Startup、I/O Variables、Ready 范围过滤，并显示 Level、Scope、Target、Evidence、Expected、Actual、Action
- 可以从命令面板打开，也会写入导出的 Markdown 诊断报告

这个能力承担“当前项目证据 vs 当前在线证据”的只读一致性门禁。它不替代 Diagnostics；EtherCAT service、`/dev/EtherCAT0`、网卡驱动、firmware/module blacklist、DKMS、`ethercatctl` 和修复建议仍然只在 Diagnostics / Host Health 中维护。后续更深的完整 Offline 配置对比还应继续围绕设备库、ESI 结构化配置、PDO 映射编辑器和参数模板展开。

### 工程文件功能

`.ecatproj` 当前会保存和恢复：

- 工程名
- 主站配置
- 在线快照
- Object Bookmarks 常用对象书签
- Watch 历史
- I/O Variables Alias/Tags/Note 元数据
- Startup SDO 列表
- ESI 仓库
- Project Notes

### 诊断和报告

当前支持：

- 诊断中心
- Host Health 主机环境检查，包括 EtherCAT service、`/dev/EtherCAT0`、网卡驱动、firmware/module blacklist、DKMS、`ethercatctl` 和修复建议
- 操作日志
- 运行时错误记录
- Free Run 状态记录
- 扫描结果记录
- SDO 操作记录
- 命令面板显式触发记录，包含操作类型和命令名
- 导出诊断报告

### 设置功能

当前设置页支持：

- 主题：Dark / Light
- 语言：English / 简体中文
- UI 缩放
- 多主站配置
- 当前活动主站保存

## 构建和运行

构建：

```bash
cmake -S . -B build
cmake --build build
```

启动 daemon：

```bash
./build/apps/ecatd/ecatd
```

启动 GUI：

```bash
./build/apps/ecat-studio/ecat-studio
```

## 当前阶段判断

当前 NekoEcat Studio 已经从简单调试 UI 进入工程工作站雏形阶段。

它已经具备成熟 EtherCAT 主站软件的基础骨架：

- runtime daemon
- GUI 工程环境
- 工程保存
- 在线诊断
- SDO/PDO 查看
- SDO 读写
- Startup SDO
- Free Run
- I/O Variables 工程信号表
- Consistency 一致性审阅
- 状态机视图
- ESI XML 管理
- 多主站配置
- 设置和关于页面

目前还没有达到 TwinCAT/System Manager 的完整深度，主要差距集中在配置编辑、设备库、工程级变量绑定和调试工具链。

## 后续优先路线

下一阶段优先补齐：

1. PDO 映射编辑器
2. 设备配置页
3. ESI 设备库结构化解析
4. 更深入的 Offline 设备库配置差异对比和 PDO 配置编辑
5. 启动流程编排
6. Free Run 波形和趋势图
7. 设备库驱动的工程级符号名绑定、批量变量命名规则和 PLC 变量导出格式
8. 从站参数模板
9. 工程级错误检查和一键诊断
10. 状态机流程编排增强，例如按设备族定义更细的前置检查模板

这些能力补完后，项目会更接近真正的 TwinCAT System Manager，而不是增强版 EtherCAT 调试工具。
