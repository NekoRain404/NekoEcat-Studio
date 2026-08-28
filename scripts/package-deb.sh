#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# Default version is derived from the CMake project; override with $1.
VERSION="${1:-$(grep -oP 'project\(NekoEcatStudio VERSION \K[0-9.]+' "${ROOT_DIR}/CMakeLists.txt")}"
PACKAGE_NAME="nekoecat-studio"
DEB_DIR="${ROOT_DIR}/dist/deb"
BUILD_DIR="${ROOT_DIR}/build"

cd "${ROOT_DIR}"

echo "==> Building release"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

echo "==> Running tests"
ctest --test-dir build --output-on-failure -j$(nproc)

echo "==> Creating DEB package structure"
DEB_PKG_DIR="${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64"
rm -rf "${DEB_PKG_DIR}"
mkdir -p "${DEB_PKG_DIR}/DEBIAN"
mkdir -p "${DEB_PKG_DIR}/usr/bin"
mkdir -p "${DEB_PKG_DIR}/usr/share/applications"
mkdir -p "${DEB_PKG_DIR}/usr/share/doc/${PACKAGE_NAME}"
mkdir -p "${DEB_PKG_DIR}/usr/share/icons/hicolor/256x256/apps"

echo "==> Copying binaries"
cp "${BUILD_DIR}/apps/ecat-studio/ecat-studio" "${DEB_PKG_DIR}/usr/bin/"
cp "${BUILD_DIR}/apps/ecatd/ecatd" "${DEB_PKG_DIR}/usr/bin/"

echo "==> Creating control file"
cat > "${DEB_PKG_DIR}/DEBIAN/control" << EOF
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Section: devel
Priority: optional
Architecture: amd64
Depends: qt6-base-dev, libqt6network6, libqt6widgets6, libqt6test6
Maintainer: NekoRain404 <nekorain404@gmail.com>
Description: NekoEcat Studio - Modern EtherCAT Engineering Workstation
 NekoEcat Studio is a modern EtherCAT engineering workstation for Linux,
 built on the IgH EtherCAT Master stack. It provides a comprehensive
 set of tools for EtherCAT commissioning, diagnostics, and monitoring.
Homepage: https://github.com/NekoRain404/NekoEcat-Studio
EOF

echo "==> Creating postinst script"
cat > "${DEB_PKG_DIR}/DEBIAN/postinst" << 'POSTINST'
#!/bin/bash
set -e

if [ "\$1" = "configure" ]; then
    echo "NekoEcat Studio has been installed."
    echo "Run 'ecat-studio' to start the GUI."
    echo "Run 'ecatd' to start the daemon."
fi
POSTINST
chmod 755 "${DEB_PKG_DIR}/DEBIAN/postinst"

echo "==> Creating desktop entry"
cat > "${DEB_PKG_DIR}/usr/share/applications/ecat-studio.desktop" << EOF
[Desktop Entry]
Name=NekoEcat Studio
Comment=EtherCAT Engineering Workstation
Exec=ecat-studio
Icon=ecat-studio
Terminal=false
Type=Application
Categories=Development;Engineering;
Keywords=EtherCAT;Industrial;Automation;
StartupWMClass=ecat-studio
EOF

echo "==> Copying documentation"
cp "${ROOT_DIR}/README.md" "${DEB_PKG_DIR}/usr/share/doc/${PACKAGE_NAME}/"
cp "${ROOT_DIR}/LICENSE" "${DEB_PKG_DIR}/usr/share/doc/${PACKAGE_NAME}/"
cp "${ROOT_DIR}/RELEASE_NOTES.md" "${DEB_PKG_DIR}/usr/share/doc/${PACKAGE_NAME}/"

echo "==> Copying icon"
if [ -f "${ROOT_DIR}/apps/ecat-studio/themes/icons/ecat-studio.png" ]; then
  cp "${ROOT_DIR}/apps/ecat-studio/themes/icons/ecat-studio.png" \
     "${DEB_PKG_DIR}/usr/share/icons/hicolor/256x256/apps/"
fi

echo "==> Building DEB package"
mkdir -p "${DEB_DIR}"
dpkg-deb --build "${DEB_PKG_DIR}" "${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64.deb"

echo "==> Generating checksum"
sha256sum "${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64.deb" \
  > "${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64.deb.sha256"

echo ""
echo "==> DEB package created"
echo "    Package: ${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64.deb"
echo "    Checksum: ${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64.deb.sha256"
echo ""
echo "    Install with: sudo dpkg -i ${DEB_DIR}/${PACKAGE_NAME}_${VERSION}_amd64.deb"
