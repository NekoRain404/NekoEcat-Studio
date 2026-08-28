#!/bin/bash
set -euo pipefail
SCRATCH=${1:-/tmp/nekoecat-goal-evidence}
mkdir -p "$SCRATCH"

echo "=== full build ===" > "$SCRATCH/build.log"
cd "$(dirname "$0")/.."
cmake -B build -DCMAKE_BUILD_TYPE=Release >> "$SCRATCH/build.log" 2>&1
cmake --build build -j$(nproc) >> "$SCRATCH/build.log" 2>&1

echo "=== full ctest run 1 ==="
cd build
ctest --test-dir . -j1 --output-on-failure 2>&1 | tee "$SCRATCH/ctest_final1.log"

# Second independent run is only claimed if it actually happened. Do not copy
# the first log and label it as a separate run.
if [ "${DUPLICATE_RUN:-0}" = "1" ]; then
  echo "=== full ctest run 2 ==="
  ctest --test-dir . -j1 --output-on-failure 2>&1 | tee "$SCRATCH/ctest_final2.log"
fi

echo "=== volume counts ===" > "$SCRATCH/cleanup_volume.log"
cd ..
echo "=== wc line counts (key realtime + ui + core) ===" >> "$SCRATCH/cleanup_volume.log"
wc -l apps/ecat-studio/plugins/*.cpp apps/ecat-studio/plugins/*/*.cpp 2>/dev/null | tail -5 >> "$SCRATCH/cleanup_volume.log" || true
wc -l apps/ecat-studio/MainWindow.cpp apps/ecat-studio/MainWindow.h apps/ecat-studio/services/ServiceContainer.cpp apps/ecat-studio/services/ServiceContainer.h apps/ecatd/FreeRunController.cpp apps/ecatd/FreeRunController.h apps/ecatd/freerun_shm_mirror.cpp apps/ecatd/freerun_shm_mirror.h client/nekoecat_client.c client/nekoecat_client.h >> "$SCRATCH/cleanup_volume.log"
echo "plugins dir du:" >> "$SCRATCH/cleanup_volume.log"
du -s apps/ecat-studio/plugins apps/ecat-studio/services apps/ecatd 2>/dev/null >> "$SCRATCH/cleanup_volume.log"
echo "plugins in cmake:" >> "$SCRATCH/cleanup_volume.log"
grep -c 'plugins/.*Plugin\.' apps/ecat-studio/CMakeLists.txt >> "$SCRATCH/cleanup_volume.log" || echo 0 >> "$SCRATCH/cleanup_volume.log"
echo "services in cmake:" >> "$SCRATCH/cleanup_volume.log"
grep -c 'services/.*Service\.' apps/ecat-studio/CMakeLists.txt >> "$SCRATCH/cleanup_volume.log" || echo 0 >> "$SCRATCH/cleanup_volume.log"
echo "registered in MainWindow:" >> "$SCRATCH/cleanup_volume.log"
grep -c 'registerPlugin(new ' apps/ecat-studio/MainWindow.cpp >> "$SCRATCH/cleanup_volume.log" || echo 0 >> "$SCRATCH/cleanup_volume.log"
echo "visible false count:" >> "$SCRATCH/cleanup_volume.log"
grep -r 'visible() const { return false; }' apps/ecat-studio/plugins --include='*.cpp' | wc -l >> "$SCRATCH/cleanup_volume.log"
echo "=== dupe inspection (repeated compute/size logic or headers) ===" >> "$SCRATCH/cleanup_volume.log"
grep -n "bitLength\|offset\|computedSize\|data_size" apps/ecatd/FreeRunController.cpp apps/ecatd/freerun_shm_mirror.cpp | head -20 >> "$SCRATCH/cleanup_volume.log" || true
echo "=== visible plugins count (should not increase) ===" >> "$SCRATCH/cleanup_volume.log"
grep -r 'visible() const { return true; }' apps/ecat-studio/plugins --include='*.cpp' | wc -l >> "$SCRATCH/cleanup_volume.log" || echo 0 >> "$SCRATCH/cleanup_volume.log"

echo "=== mirror test (internal 2x repeat inside shipped test) ===" > "$SCRATCH/mirror_test.log"
cd build
./tests/unit/core/free_run_mirror_test -v2 >> "$SCRATCH/mirror_test.log" 2>&1 || true

echo "=== client c api (layout test exercising connect/attach/wait/by_index/state) ===" > "$SCRATCH/client_c_api.log"
# run the layout test binary (pure-C consumer; does connect/attach or fallback, full surface per plan step 3)
LAYOUT_BIN="./tests/unit/core/nekoecat_client_layout_test"
if [ -x "$LAYOUT_BIN" ]; then
  echo "using layout test binary" >> "$SCRATCH/client_c_api.log"
else
  gcc -std=c99 -w -Iclient -o "$SCRATCH/layout_test" tests/unit/core/nekoecat_client_layout_test.c client/nekoecat_client.c -pthread 2>/dev/null || true
  LAYOUT_BIN="$SCRATCH/layout_test"
fi
echo "=== RUN 1 ===" >> "$SCRATCH/client_c_api.log"
if [ -x "$LAYOUT_BIN" ]; then "$LAYOUT_BIN" >> "$SCRATCH/client_c_api.log" 2>&1 || true; else echo "no layout bin" >> "$SCRATCH/client_c_api.log"; fi
echo "=== RUN 2 ===" >> "$SCRATCH/client_c_api.log"
if [ -x "$LAYOUT_BIN" ]; then "$LAYOUT_BIN" >> "$SCRATCH/client_c_api.log" 2>&1 || true; else echo "no layout bin" >> "$SCRATCH/client_c_api.log"; fi

# prefer the real shm_rpc from the running product; fallback placeholder only if absent
SHM_RPC_SRC="${SHM_RPC_SRC:-$SCRATCH/shm_rpc.json}"
if [ ! -s "$SHM_RPC_SRC" ]; then
  echo '{"shm_name":"/nekoecat_proc_0","data_size":0,"layout_version":0,"layout":[]}' > "$SCRATCH/shm_rpc.json"
elif [ "$SHM_RPC_SRC" != "$SCRATCH/shm_rpc.json" ]; then
  cp "$SHM_RPC_SRC" "$SCRATCH/shm_rpc.json"
fi

echo "capture done to $SCRATCH"
