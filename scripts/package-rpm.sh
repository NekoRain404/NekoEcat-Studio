#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${1:-1.2.0}"
PACKAGE_NAME="nekoecat-studio"
RPM_DIR="${ROOT_DIR}/dist/rpm"
BUILD_DIR="${ROOT_DIR}/build"

cd "${ROOT_DIR}"

echo "==> Building release"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

echo "==> Running tests"
ctest --test-dir build --output-on-failure -j$(nproc)

echo "==> Setting up RPM build environment"
RPM_BUILD_DIR="${RPM_DIR}/rpmbuild"
rm -rf "${RPM_BUILD_DIR}"
mkdir -p "${RPM_BUILD_DIR}"/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

echo "==> Creating source tarball"
TARBALL="${PACKAGE_NAME}-${VERSION}.tar.gz"
tar -czf "${RPM_BUILD_DIR}/SOURCES/${TARBALL}" \
  --exclude=build --exclude=dist --exclude=.git \
  --transform "s,^\.,${PACKAGE_NAME}-${VERSION}," \
  -C "${ROOT_DIR}" \
  .

echo "==> Creating spec file"
cat > "${RPM_BUILD_DIR}/SPECS/${PACKAGE_NAME}.spec" << EOF
Name:           ${PACKAGE_NAME}
Version:        ${VERSION}
Release:        1%{?dist}
Summary:        Modern EtherCAT Engineering Workstation
License:        GPLv3
URL:            https://github.com/NekoRain404/NekoEcat-Studio
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake >= 3.20
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel

Requires:       qt6-qtbase
Requires:       qt6-qtnetwork

%description
NekoEcat Studio is a modern EtherCAT engineering workstation for Linux,
built on the IgH EtherCAT Master stack. It provides a comprehensive
set of tools for EtherCAT commissioning, diagnostics, and monitoring.

%prep
%autosetup -n %{name}-%{version}

%build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%{_prefix}
cmake --build build -j\$(nproc)

%install
rm -rf %{buildroot}
cmake --install build --prefix %{buildroot}%{_prefix}

%files
%license LICENSE
%doc README.md RELEASE_NOTES.md
%{_bindir}/ecat-studio
%{_bindir}/ecatd
%{_datadir}/applications/ecat-studio.desktop
%{_datadir}/icons/hicolor/256x256/apps/ecat-studio.png

%changelog
* $(date '+%a %b %d %Y') NekoRain404 <nekorain404@gmail.com> - ${VERSION}-1
- Release ${VERSION}
EOF

echo "==> Building RPM package"
rpmbuild -bb "${RPM_BUILD_DIR}/SPECS/${PACKAGE_NAME}.spec" \
  --define "_topdir ${RPM_BUILD_DIR}"

echo "==> Copying RPM to dist"
mkdir -p "${RPM_DIR}"
cp "${RPM_BUILD_DIR}"/RPMS/*/*.rpm "${RPM_DIR}/"

echo "==> Generating checksum"
cd "${RPM_DIR}"
for rpm_file in *.rpm; do
  sha256sum "${rpm_file}" > "${rpm_file}.sha256"
done

echo ""
echo "==> RPM package created"
echo "    Packages in: ${RPM_DIR}/"
ls -la "${RPM_DIR}/"*.rpm 2>/dev/null || true
echo ""
echo "    Install with: sudo rpm -ivh ${RPM_DIR}/${PACKAGE_NAME}-${VERSION}-1.*.rpm"
