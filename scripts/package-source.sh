#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# Default version is derived from the CMake project; override with $1.
VERSION="${1:-$(grep -oP 'project\(NekoEcatStudio VERSION \K[0-9.]+' "${ROOT_DIR}/CMakeLists.txt")}"
PACKAGE_NAME="NekoEcat-Studio"
SOURCE_DIR="${ROOT_DIR}/dist/source"

cd "${ROOT_DIR}"

echo "==> Creating source package structure"
SRC_PKG_DIR="${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src"
rm -rf "${SRC_PKG_DIR}"
mkdir -p "${SRC_PKG_DIR}"

echo "==> Copying source files"
# Copy source directories
cp -r src "${SRC_PKG_DIR}/"
cp -r apps "${SRC_PKG_DIR}/"
cp -r tests "${SRC_PKG_DIR}/"
cp -r scripts "${SRC_PKG_DIR}/"
cp -r tools "${SRC_PKG_DIR}/"
cp -r translations "${SRC_PKG_DIR}/"
cp -r docs "${SRC_PKG_DIR}/"
cp -r .github "${SRC_PKG_DIR}/"

# Copy root files
cp CMakeLists.txt "${SRC_PKG_DIR}/"
cp README.md "${SRC_PKG_DIR}/"
cp LICENSE "${SRC_PKG_DIR}/"
cp CHANGELOG.md "${SRC_PKG_DIR}/"
cp RELEASE_NOTES.md "${SRC_PKG_DIR}/"
cp Doxyfile "${SRC_PKG_DIR}/"
cp .gitignore "${SRC_PKG_DIR}/"

echo "==> Removing build artifacts"
find "${SRC_PKG_DIR}" -type d -name "build" -exec rm -rf {} + 2>/dev/null || true
find "${SRC_PKG_DIR}" -type d -name ".git" -exec rm -rf {} + 2>/dev/null || true
find "${SRC_PKG_DIR}" -name "*.o" -delete 2>/dev/null || true
find "${SRC_PKG_DIR}" -name "*.so" -delete 2>/dev/null || true
find "${SRC_PKG_DIR}" -name "*.a" -delete 2>/dev/null || true

echo "==> Creating source tarball"
mkdir -p "${SOURCE_DIR}"
tar -czf "${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.tar.gz" \
  -C "${SOURCE_DIR}" "${PACKAGE_NAME}-${VERSION}-src"

echo "==> Generating checksum"
sha256sum "${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.tar.gz" \
  > "${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.tar.gz.sha256"

echo "==> Creating zip archive"
if command -v zip &>/dev/null; then
  cd "${SOURCE_DIR}"
  zip -r "${PACKAGE_NAME}-${VERSION}-src.zip" "${PACKAGE_NAME}-${VERSION}-src"
  sha256sum "${PACKAGE_NAME}-${VERSION}-src.zip" \
    > "${PACKAGE_NAME}-${VERSION}-src.zip.sha256"
  cd "${ROOT_DIR}"
fi

echo ""
echo "==> Source package created"
echo "    Tarball: ${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.tar.gz"
if [ -f "${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.zip" ]; then
  echo "    Zip:     ${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.zip"
fi
echo ""
echo "    Extract: tar -xzf ${SOURCE_DIR}/${PACKAGE_NAME}-${VERSION}-src.tar.gz"
