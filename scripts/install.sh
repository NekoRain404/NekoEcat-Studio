#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "==> Detecting package manager"
if command -v apt-get &>/dev/null; then
  PKG_MGR="apt"
elif command -v dnf &>/dev/null; then
  PKG_MGR="dnf"
elif command -v pacman &>/dev/null; then
  PKG_MGR="pacman"
else
  echo "Error: No supported package manager found (apt, dnf, pacman)"
  exit 1
fi

echo "==> Using package manager: ${PKG_MGR}"

echo "==> Installing build dependencies"
case "${PKG_MGR}" in
  apt)
    sudo apt-get update
    sudo apt-get install -y build-essential cmake \
      qt6-base-dev qt6-tools-dev qt6-l10n-tools \
      libqt6network6 libqt6widgets6 libqt6test6
    ;;
  dnf)
    sudo dnf install -y gcc gcc-c++ cmake make \
      qt6-qtbase-devel qt6-qttools-devel
    ;;
  pacman)
    sudo pacman -S --needed --noconfirm base-devel cmake \
      qt6-base qt6-tools
    ;;
esac

echo "==> Configuring project"
cmake -B build -DCMAKE_BUILD_TYPE=Release

echo "==> Building project"
cmake --build build -j$(nproc)

echo "==> Installing project"
sudo cmake --install build

echo "==> Creating desktop entry"
DESKTOP_DIR="${HOME}/.local/share/applications"
mkdir -p "${DESKTOP_DIR}"

ICON_PATH="/usr/local/share/icons/hicolor/256x256/apps/ecat-studio.png"
if [ ! -f "${ICON_PATH}" ]; then
  ICON_PATH="utilities-terminal"
fi

cat > "${DESKTOP_DIR}/ecat-studio.desktop" << EOF
[Desktop Entry]
Name=NekoEcat Studio
Comment=EtherCAT Engineering Workstation
Exec=ecat-studio
Icon=${ICON_PATH}
Terminal=false
Type=Application
Categories=Development;Engineering;
Keywords=EtherCAT;Industrial;Automation;
StartupWMClass=ecat-studio
EOF

echo "==> Done! NekoEcat Studio has been installed."
echo "    Run 'ecat-studio' or find it in your application menu."
