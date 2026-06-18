# NekoEcat Studio v1.0.0

**Release Date:** 2026-06-18

## 新功能 / New Features

### 插件架构 / Plugin Architecture
- WorkspacePlugin 接口 + EventBus 事件总线 + Service 服务层
- 每个工作区独立为插件，便于扩展和维护
- PluginRegistry 管理插件注册、排序和可见性

### DC Sync 分布式时钟诊断 / DC Sync Diagnostics
- 参考时钟检测和指示
- 每从站 DC 同步状态（同步中/未同步/错误）
- 漂移和抖动统计（最小/最大/平均）
- 自动轮询更新

### AL Event 应用层事件日志 / AL Event Log
- 按时间顺序的事件表格
- 严重级别过滤（错误/警告/信息）
- 自动滚动 + 暂停悬停
- 事件清除功能

### Signal Analyzer 实时信号分析 / Real-time Signal Analysis
- 多通道信号采集
- QPainter 反锯齿滚动图表
- 10,000 点环形缓冲
- 通道统计（最小/最大/平均/标准差）
- 可配置窗口大小（100-10000 点）

### Network Adapter 网络适配器选择 / Adapter Selection
- 自动检测可用网卡
- 显示接口名称、MAC、驱动、链路状态
- 一键切换 IgH 主站适配器

### 自动重连 / Auto-Reconnect
- 心跳检测（5 秒间隔）
- 指数退避重连（2→30 秒）
- 状态栏指示器

## 测试覆盖 / Test Coverage
- 66 个单元测试全部通过
- 覆盖：模型层、服务层、守护进程、GUI 冒烟
- TDD 驱动开发

## 系统要求 / System Requirements
- Linux (Arch/Ubuntu/Debian)
- Qt6 (Core, Network, Widgets, Test)
- IgH EtherCAT Master
- CMake 3.20+

## 安装 / Installation

```bash
# Arch Linux (AUR)
yay -NekoEcat-Studio

# 从源码构建 / Build from source
git clone https://github.com/NekoRain404/NekoEcat-Studio.git
cd NekoEcat-Studio
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
sudo cmake --install build
```

## 使用 / Usage

```bash
# 启动守护进程 / Start daemon
ecatd &

# 启动 GUI / Start GUI
ecat-studio
```

## 已知限制 / Known Limitations
- 仅支持 Linux 平台
- 需要 IgH EtherCAT Master 内核模块
- 部分功能需要 root 权限

## 下一步 / Next Steps
- 完善工作区插件化转换
- 添加更多 TwinCAT 对标功能
- 优化性能和用户体验

---

**Full Changelog**: https://github.com/NekoRain404/NekoEcat-Studio/commits/v1.0.0
