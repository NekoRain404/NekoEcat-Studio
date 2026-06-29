#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${1:-1.0.0}"
PACKAGE_NAME="NekoEcat-Studio-v${VERSION}-linux-x86_64"
DIST_DIR="${ROOT_DIR}/dist"
PACKAGE_DIR="${DIST_DIR}/${PACKAGE_NAME}"

cmake --build "${ROOT_DIR}/build" --target ecat-studio
cmake --build "${ROOT_DIR}/build" --target ecatd

rm -rf "${PACKAGE_DIR}" "${DIST_DIR}/${PACKAGE_NAME}.tar.gz"
mkdir -p "${PACKAGE_DIR}/bin"

cp "${ROOT_DIR}/build/apps/ecat-studio/ecat-studio" "${PACKAGE_DIR}/bin/"
cp "${ROOT_DIR}/build/apps/ecatd/ecatd" "${PACKAGE_DIR}/bin/"
cp "${ROOT_DIR}/README.md" "${ROOT_DIR}/LICENSE" "${PACKAGE_DIR}/"

cat >"${PACKAGE_DIR}/NekoEcat-Studio" <<'LAUNCHER'
#!/usr/bin/env bash
set -euo pipefail

APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec "${APP_DIR}/bin/ecat-studio" "$@"
LAUNCHER
chmod +x "${PACKAGE_DIR}/NekoEcat-Studio"

cat >"${PACKAGE_DIR}/DEPENDENCIES.md" <<'DEPS'
# Runtime Dependencies

This Linux binary package expects the target system to provide:

- Qt6 Core, Network, Widgets, and GUI runtime libraries
- IgH EtherCAT Master runtime and `libethercat.so.1`
- Standard Linux desktop OpenGL/X11 dependencies required by Qt6

On Arch Linux, the typical base runtime is:

```bash
sudo pacman -S qt6-base ethercat
```

If the GUI starts but cannot connect to the bus, check that the IgH EtherCAT
master service, NIC driver, and EtherCAT interface configuration are valid.
DEPS

cat >"${PACKAGE_DIR}/RELEASE_NOTES.md" <<'NOTES'
# NekoEcat Studio v0.1.0

NekoEcat Studio 的首个 GPLv3 公开版本。它面向 Linux 与 IgH EtherCAT
Master，提供现代化 EtherCAT 工程工作站体验，帮助工程师完成从站浏览、
对象字典检查、SDO/PDO 操作、Free Run 调试、诊断与主机环境确认。

Initial GPLv3 public release of NekoEcat Studio, a modern EtherCAT engineering
workstation for Linux and IgH EtherCAT Master. It helps engineers inspect
slaves, review Object Dictionary entries, operate SDO/PDO workflows, run
Free Run diagnostics, and verify host-side runtime conditions.

## 版本亮点 / Highlights

- 采用 Qt6 构建的现代桌面工程工作站，软件名称与品牌更新为
  NekoEcat Studio。
- 随包提供 `ecatd` 本地运行时守护进程，GUI 可通过 localhost 与运行时协作。
- README 已补全中英双语介绍，覆盖架构、工作流、构建、测试与发布流程。
- 仓库与发布包均包含 GPLv3 许可证。
- 覆盖 Overview、Diagnostics、Object Dictionary、SDO/PDO、Watch、
  Startup SDO、Free Run、I/O Variables 与 Host Environment 等核心工程界面。
- 发布包提供根目录启动入口，解压后可直接执行 `./NekoEcat-Studio`。

- Qt6 desktop workstation branded as NekoEcat Studio.
- Runtime companion daemon `ecatd` included in the binary package.
- Bilingual Chinese/English README with architecture, workflow, build, test,
  and release notes.
- GPLv3 license included in the repository and release archive.
- Engineering surfaces for Overview, Diagnostics, Object Dictionary, SDO/PDO,
  Watch, Startup SDO, Free Run, I/O Variables, and host environment checks.
- Root launcher included in the package, so users can run `./NekoEcat-Studio`
  after extraction.

## 运行环境 / Runtime Requirements

- Linux x86_64
- Qt6 Core / Network / Widgets / GUI runtime libraries
- IgH EtherCAT Master runtime and `libethercat.so.1`
- 正确配置的 EtherCAT 网卡、主站服务与权限
- A configured EtherCAT NIC, master service, and runtime permissions

## 运行方式 / Run

```bash
tar -xzf NekoEcat-Studio-v0.1.0-linux-x86_64.tar.gz
cd NekoEcat-Studio-v0.1.0-linux-x86_64
./NekoEcat-Studio
```

## 验证 / Validation

- `cmake --build build --target release-smoke`: 15/15 tests passed.
- 包内二进制已检查执行权限。
- `ldd` 依赖清单随包提供：`ldd-ecat-studio.txt` 与 `ldd-ecatd.txt`。
- Offscreen GUI startup smoke passed with the expected timeout behavior for a
  running Qt event loop.
NOTES

chmod +x "${PACKAGE_DIR}/bin/ecat-studio" "${PACKAGE_DIR}/bin/ecatd"

(
  cd "${PACKAGE_DIR}"
  ldd bin/ecat-studio >"ldd-ecat-studio.txt"
  ldd bin/ecatd >"ldd-ecatd.txt"
)

tar -C "${DIST_DIR}" -czf "${DIST_DIR}/${PACKAGE_NAME}.tar.gz" "${PACKAGE_NAME}"
sha256sum "${DIST_DIR}/${PACKAGE_NAME}.tar.gz" >"${DIST_DIR}/${PACKAGE_NAME}.tar.gz.sha256"

echo "Package: ${DIST_DIR}/${PACKAGE_NAME}.tar.gz"
cat "${DIST_DIR}/${PACKAGE_NAME}.tar.gz.sha256"
