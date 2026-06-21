#!/usr/bin/env bash
set -euo pipefail

if [ $# -lt 1 ]; then
  echo "Usage: $0 <version>"
  echo "Example: $0 1.0.0"
  exit 1
fi

VERSION="$1"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PACKAGE_NAME="NekoEcat-Studio-v${VERSION}-linux-x86_64"

cd "${ROOT_DIR}"

echo "==> Building release for version ${VERSION}"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

echo "==> Running tests"
ctest --test-dir build --output-on-failure -j$(nproc)

echo "==> Creating release package"
bash scripts/package-linux.sh "${VERSION}"

echo "==> Creating git tag v${VERSION}"
if git rev-parse "v${VERSION}" >/dev/null 2>&1; then
  echo "Tag v${VERSION} already exists, skipping tag creation."
else
  git tag -a "v${VERSION}" -m "Release v${VERSION}"
fi

echo "==> Creating GitHub release"
if ! command -v gh &>/dev/null; then
  echo "gh CLI not found. Install it with: sudo apt-get install gh"
  echo "Then run: gh release create v${VERSION} dist/${PACKAGE_NAME}.tar.gz dist/${PACKAGE_NAME}.tar.gz.sha256"
  exit 1
fi

gh release create "v${VERSION}" \
  --title "NekoEcat Studio v${VERSION}" \
  --generate-notes \
  "dist/${PACKAGE_NAME}.tar.gz" \
  "dist/${PACKAGE_NAME}.tar.gz.sha256"

echo "==> Done! Release v${VERSION} published."
