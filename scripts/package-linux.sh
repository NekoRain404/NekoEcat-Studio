#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${1:-0.1.0}"
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

Initial GPLv3 release of NekoEcat Studio, a modern EtherCAT engineering
workstation for Linux and IgH EtherCAT Master.

## Highlights

- Qt6 desktop workstation branded as NekoEcat Studio.
- Runtime companion daemon `ecatd` included in the binary package.
- Bilingual Chinese/English README with architecture, workflow, build, test,
  and release notes.
- GPLv3 license included in the repository and release archive.
- Engineering surfaces for Overview, Diagnostics, Object Dictionary, SDO/PDO,
  Watch, Startup SDO, Free Run, I/O Variables, and host environment checks.

## Run

```bash
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
