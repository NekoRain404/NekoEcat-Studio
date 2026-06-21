#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${1:-1.2.0}"
PACKAGE_NAME="NekoEcat-Studio"
APPIMAGE_DIR="${ROOT_DIR}/dist/appimage"
BUILD_DIR="${ROOT_DIR}/build"

cd "${ROOT_DIR}"

echo "==> Building release"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

echo "==> Running tests"
ctest --test-dir build --output-on-failure -j$(nproc)

echo "==> Setting up AppImage structure"
APPDIR="${APPIMAGE_DIR}/${PACKAGE_NAME}.AppDir"
rm -rf "${APPDIR}"
mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/lib"
mkdir -p "${APPDIR}/usr/share/applications"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"

echo "==> Copying binaries"
cp "${BUILD_DIR}/apps/ecat-studio/ecat-studio" "${APPDIR}/usr/bin/"
cp "${BUILD_DIR}/apps/ecatd/ecatd" "${APPDIR}/usr/bin/"

echo "==> Copying libraries"
# Copy required Qt libraries
for lib in libQt6Core.so libQt6Network.so libQt6Widgets.so libQt6Gui.so; do
  if [ -f "/usr/lib/${lib}" ]; then
    cp "/usr/lib/${lib}" "${APPDIR}/usr/lib/"
  elif [ -f "/usr/lib/x86_64-linux-gnu/${lib}" ]; then
    cp "/usr/lib/x86_64-linux-gnu/${lib}" "${APPDIR}/usr/lib/"
  fi
done

echo "==> Creating desktop entry"
cat > "${APPDIR}/usr/share/applications/ecat-studio.desktop" << EOF
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

cp "${APPDIR}/usr/share/applications/ecat-studio.desktop" "${APPDIR}/"

echo "==> Copying icon"
if [ -f "${ROOT_DIR}/apps/ecat-studio/themes/icons/ecat-studio.png" ]; then
  cp "${ROOT_DIR}/apps/ecat-studio/themes/icons/ecat-studio.png" \
     "${APPDIR}/usr/share/icons/hicolor/256x256/apps/"
  cp "${APPDIR}/usr/share/icons/hicolor/256x256/apps/ecat-studio.png" "${APPDIR}/"
fi

echo "==> Creating AppRun"
cat > "${APPDIR}/AppRun" << 'APPRUN'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
exec "${HERE}/usr/bin/ecat-studio" "$@"
APPRUN
chmod +x "${APPDIR}/AppRun"

echo "==> Creating AppImage"
if command -v appimagetool &>/dev/null; then
  appimagetool "${APPDIR}" "${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage"
else
  echo "Warning: appimagetool not found. Install it from https://github.com/AppImage/AppImageKit"
  echo "  Or download: https://github.com/AppImage/AppImageKit/releases/latest/download/appimagetool-x86_64.AppImage"
  echo ""
  echo "  Manual build:"
  echo "    chmod +x appimagetool-x86_64.AppImage"
  echo "    ./appimagetool-x86_64.AppImage ${APPDIR} ${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage"
fi

echo "==> Generating checksum"
if [ -f "${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage" ]; then
  sha256sum "${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage" \
    > "${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage.sha256"
fi

echo ""
echo "==> AppImage package created"
echo "    Package: ${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage"
echo ""
echo "    Run with: chmod +x ${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage"
echo "              ${APPIMAGE_DIR}/${PACKAGE_NAME}-${VERSION}-x86_64.AppImage"
