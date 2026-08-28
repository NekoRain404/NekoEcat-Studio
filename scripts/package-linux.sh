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

cat >"${PACKAGE_DIR}/RELEASE_NOTES.md" <<NOTES
# NekoEcat Studio v${VERSION}

NekoEcat Studio is a modern EtherCAT engineering workstation for Linux and the
IgH EtherCAT Master. It helps engineers inspect slaves, review Object
Dictionary entries, operate SDO/PDO workflows, run Free Run diagnostics, and
verify host-side runtime conditions.

For full release history, features, build instructions, and test information,
see the bundled `CHANGELOG.md` and `README.md`.

## 运行环境 / Runtime Requirements

- Linux x86_64
- Qt6 Core / Network / Widgets / GUI runtime libraries
- IgH EtherCAT Master runtime and `libethercat.so.1`
- A configured EtherCAT NIC, master service, and runtime permissions

## 运行方式 / Run

```bash
tar -xzf NekoEcat-Studio-v${VERSION}-linux-x86_64.tar.gz
cd NekoEcat-Studio-v${VERSION}-linux-x86_64
./NekoEcat-Studio
```
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
