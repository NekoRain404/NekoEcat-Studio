#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-bench"
REPORT_DIR="${ROOT_DIR}/build/reports/benchmark"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
ITERATIONS=5

for arg in "$@"; do
  case "${arg}" in
    --iterations=*) ITERATIONS="${arg#*=}" ;;
    --help|-h)
      echo "Usage: $0 [--iterations=N]"
      echo "  Runs performance benchmarks and generates reports."
      echo "  Default iterations: ${ITERATIONS}"
      exit 0
      ;;
  esac
done

cd "${ROOT_DIR}"
mkdir -p "${REPORT_DIR}"

echo "==> Configuring (Release)"
cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release

echo "==> Building"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "==> Running benchmarks (${ITERATIONS} iterations)"

RESULTS_JSON="${REPORT_DIR}/results_${TIMESTAMP}.json"
cat > "${RESULTS_JSON}" <<'HEADER'
{
  "timestamp": "TIMESTAMP_PLACEHOLDER",
  "benchmarks": {
HEADER

FIRST=true

bench_startup() {
  local times=()
  for i in $(seq 1 "${ITERATIONS}"); do
    local start end elapsed
    start=$(date +%s%N)
    ctest --test-dir "${BUILD_DIR}" --output-on-failure -j"$(nproc)" >/dev/null 2>&1 || true
    end=$(date +%s%N)
    elapsed=$(( (end - start) / 1000000 ))
    times+=("${elapsed}")
  done
  local sum=0
  for t in "${times[@]}"; do sum=$((sum + t)); done
  local avg=$((sum / ITERATIONS))
  local min=${times[0]}
  local max=${times[0]}
  for t in "${times[@]}"; do
    ((t < min)) && min=$t
    ((t > max)) && max=$t
  done
  echo "${avg} ${min} ${max}"
}

bench_memory() {
  local peak_rss=0
  for test_bin in "${BUILD_DIR}"/tests/*_test; do
    [ -x "${test_bin}" ] || continue
    local rss
    rss=$(/usr/bin/time -v "${test_bin}" 2>&1 | \
      grep "Maximum resident" | awk '{print $6}' 2>/dev/null || echo "0")
    ((rss > peak_rss)) && peak_rss=$rss
  done
  echo "${peak_rss}"
}

bench_io() {
  local test_dir="${BUILD_DIR}/bench_io"
  mkdir -p "${test_dir}"
  local start end elapsed bytes rate
  start=$(date +%s%N)
  dd if=/dev/zero of="${test_dir}/testfile" bs=1M count=100 oflag=direct 2>/dev/null || \
  dd if=/dev/zero of="${test_dir}/testfile" bs=1M count=100 2>/dev/null
  end=$(date +%s%N)
  elapsed=$(( (end - start) / 1000000 ))
  bytes=104857600
  if [ "${elapsed}" -gt 0 ]; then
    rate=$(( bytes / elapsed / 1024 ))
  else
    rate=0
  fi
  rm -rf "${test_dir}"
  echo "${rate} ${elapsed}"
}

echo "  Running startup benchmark..."
STARTUP=$(bench_startup)
STARTUP_AVG=$(echo "${STARTUP}" | awk '{print $1}')
STARTUP_MIN=$(echo "${STARTUP}" | awk '{print $2}')
STARTUP_MAX=$(echo "${STARTUP}" | awk '{print $3}')

if [ "${FIRST}" = true ]; then FIRST=false; else echo "," >> "${RESULTS_JSON}"; fi
cat >> "${RESULTS_JSON}" <<EOF
    "startup": {
      "avg_ms": ${STARTUP_AVG},
      "min_ms": ${STARTUP_MIN},
      "max_ms": ${STARTUP_MAX},
      "iterations": ${ITERATIONS}
    }
EOF
echo "  Startup: avg=${STARTUP_AVG}ms, min=${STARTUP_MIN}ms, max=${STARTUP_MAX}ms"

echo "  Running memory benchmark..."
MEM_KB=$(bench_memory)
MEM_MB=$((MEM_KB / 1024))
cat >> "${RESULTS_JSON}" <<EOF
,
    "memory": {
      "peak_rss_kb": ${MEM_KB},
      "peak_rss_mb": ${MEM_MB}
    }
EOF
echo "  Peak RSS: ${MEM_KB}KB (${MEM_MB}MB)"

echo "  Running I/O benchmark..."
IO=$(bench_io)
IO_RATE=$(echo "${IO}" | awk '{print $1}')
IO_TIME=$(echo "${IO}" | awk '{print $2}')
cat >> "${RESULTS_JSON}" <<EOF
,
    "io": {
      "throughput_kbps": ${IO_RATE},
      "elapsed_ms": ${IO_TIME}
    }
EOF
echo "  I/O throughput: ${IO_RATE}KB/s (${IO_TIME}ms for 100MB)"

cat >> "${RESULTS_JSON}" <<EOF
  },
  "environment": {
    "cpus": $(nproc),
    "hostname": "$(hostname)",
    "kernel": "$(uname -r)"
  }
}
EOF

sed -i "s/TIMESTAMP_PLACEHOLDER/${TIMESTAMP}/" "${RESULTS_JSON}"

echo "==> Generating trend data"
TREND_FILE="${REPORT_DIR}/trend.csv"
if [ ! -f "${TREND_FILE}" ]; then
  echo "timestamp,startup_avg_ms,peak_rss_kb,io_throughput_kbps" > "${TREND_FILE}"
fi
echo "${TIMESTAMP},${STARTUP_AVG},${MEM_KB},${IO_RATE}" >> "${TREND_FILE}"

echo "==> Generating HTML report"
cat > "${REPORT_DIR}/benchmark_${TIMESTAMP}.html" <<HTMLEOF
<!DOCTYPE html>
<html>
<head>
<title>NekoEcat Studio Benchmark Report</title>
<style>
  body { font-family: system-ui, sans-serif; max-width: 900px; margin: 40px auto; padding: 0 20px; }
  table { border-collapse: collapse; width: 100%; margin: 20px 0; }
  th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }
  th { background: #f5f5f5; }
  .metric { font-size: 24px; font-weight: bold; color: #2563eb; }
  h1 { border-bottom: 2px solid #2563eb; padding-bottom: 10px; }
  .section { margin: 30px 0; }
  .good { color: #16a34a; }
  .warn { color: #d97706; }
</style>
</head>
<body>
<h1>NekoEcat Studio Benchmark Report</h1>
<p>Generated: ${TIMESTAMP}</p>
<p>Iterations: ${ITERATIONS} | CPUs: $(nproc)</p>

<div class="section">
<h2>Startup Performance</h2>
<table>
  <tr><th>Metric</th><th>Value</th></tr>
  <tr><td>Average</td><td class="metric">${STARTUP_AVG} ms</td></tr>
  <tr><td>Best</td><td>${STARTUP_MIN} ms</td></tr>
  <tr><td>Worst</td><td>${STARTUP_MAX} ms</td></tr>
</table>
</div>

<div class="section">
<h2>Memory Usage</h2>
<table>
  <tr><th>Metric</th><th>Value</th></tr>
  <tr><td>Peak RSS</td><td class="metric">${MEM_MB} MB</td></tr>
</table>
</div>

<div class="section">
<h2>I/O Throughput</h2>
<table>
  <tr><th>Metric</th><th>Value</th></tr>
  <tr><td>Write Throughput</td><td class="metric">${IO_RATE} KB/s</td></tr>
  <tr><td>100MB Write Time</td><td>${IO_TIME} ms</td></tr>
</table>
</div>

<div class="section">
<h2>Trend</h2>
<pre>$(cat "${TREND_FILE}" 2>/dev/null || echo "No trend data yet")</pre>
</div>
</body>
</html>
HTMLEOF

ln -sf "${REPORT_DIR}/benchmark_${TIMESTAMP}.html" "${REPORT_DIR}/latest.html"

echo ""
echo "==> Benchmark complete"
echo "    JSON results: ${RESULTS_JSON}"
echo "    HTML report:  ${REPORT_DIR}/benchmark_${TIMESTAMP}.html"
echo "    Latest:       ${REPORT_DIR}/latest.html"
echo "    Trend:        ${REPORT_DIR}/trend.csv"

rm -rf "${BUILD_DIR}"
