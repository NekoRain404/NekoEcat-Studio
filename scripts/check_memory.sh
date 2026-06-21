#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-valgrind"
REPORT_DIR="${ROOT_DIR}/build/reports/memory"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
SUPPRESSIONS="${ROOT_DIR}/tests/valgrind.supp"

for arg in "$@"; do
  case "${arg}" in
    --help|-h)
      echo "Usage: $0"
      echo "  Runs tests under Valgrind and generates memory leak reports."
      exit 0
      ;;
  esac
done

cd "${ROOT_DIR}"

if ! command -v valgrind &>/dev/null; then
  echo "Error: valgrind not found. Install with:"
  echo "  sudo apt-get install valgrind"
  exit 1
fi

echo "==> Configuring (Debug)"
cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug

echo "==> Building"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "==> Running Valgrind analysis"
mkdir -p "${REPORT_DIR}/logs"

TOTAL_TESTS=0
LEAK_TESTS=0
CLEAN_TESTS=0
ERROR_TESTS=0
TOTAL_LEAKS=0
TOTAL_LOST=0

SUMMARY_FILE="${REPORT_DIR}/summary_${TIMESTAMP}.txt"
{
  echo "NekoEcat Studio Memory Leak Report"
  echo "==================================="
  echo "Timestamp: ${TIMESTAMP}"
  echo ""
} > "${SUMMARY_FILE}"

for test_bin in "${BUILD_DIR}"/tests/*_test; do
  [ -x "${test_bin}" ] || continue
  test_name=$(basename "${test_bin}")
  TOTAL_TESTS=$((TOTAL_TESTS + 1))

  LOG="${REPORT_DIR}/logs/${test_name}.log"
  echo "  Checking ${test_name}..."

  VG_ARGS=(
    --leak-check=full
    --show-leak-kinds=all
    --track-origins=yes
    --num-callers=20
    --log-file="${LOG}"
    --error-exitcode=2
  )
  if [ -f "${SUPPRESSIONS}" ]; then
    VG_ARGS+=(--suppressions="${SUPPRESSIONS}")
  fi

  set +e
  valgrind "${VG_ARGS[@]}" "${test_bin}" >/dev/null 2>&1
  EXIT_CODE=$?
  set -e

  DEFINITELY=$(grep -oP 'definitely lost: \K[0-9,]+' "${LOG}" 2>/dev/null | tr -d ',' || echo "0")
  INDIRECTLY=$(grep -oP 'indirectly lost: \K[0-9,]+' "${LOG}" 2>/dev/null | tr -d ',' || echo "0")
  POSSIBLY=$(grep -oP 'possibly lost: \K[0-9,]+' "${LOG}" 2>/dev/null | tr -d ',' || echo "0")
  ERRORS=$(grep -oP 'ERROR SUMMARY: \K[0-9]+' "${LOG}" 2>/dev/null || echo "0")

  DEFINITELY=${DEFINITELY:-0}
  INDIRECTLY=${INDIRECTLY:-0}
  POSSIBLY=${POSSIBLY:-0}
  ERRORS=${ERRORS:-0}

  TOTAL_LOST=$((TOTAL_LOST + DEFINITELY + INDIRECTLY))

  if [ "${EXIT_CODE}" -eq 2 ] || [ "${DEFINITELY}" -gt 0 ]; then
    LEAK_TESTS=$((LEAK_TESTS + 1))
    STATUS="LEAK"
  elif [ "${ERRORS}" -gt 0 ]; then
    ERROR_TESTS=$((ERROR_TESTS + 1))
    STATUS="ERROR"
  else
    CLEAN_TESTS=$((CLEAN_TESTS + 1))
    STATUS="OK"
  fi

  {
    echo "[${STATUS}] ${test_name}"
    echo "  definitely lost: ${DEFINITELY} bytes"
    echo "  indirectly lost: ${INDIRECTLY} bytes"
    echo "  possibly lost:   ${POSSIBLY} bytes"
    echo "  errors:          ${ERRORS}"
    echo ""
  } >> "${SUMMARY_FILE}"
done

{
  echo "==================================="
  echo "Summary"
  echo "==================================="
  echo "Total tests:      ${TOTAL_TESTS}"
  echo "Clean:            ${CLEAN_TESTS}"
  echo "With leaks:       ${LEAK_TESTS}"
  echo "With errors:      ${ERROR_TESTS}"
  echo "Total bytes lost: ${TOTAL_LOST}"
} >> "${SUMMARY_FILE}"

echo "==> Generating fix suggestions"
SUGGESTIONS_FILE="${REPORT_DIR}/suggestions_${TIMESTAMP}.txt"
{
  echo "Memory Fix Suggestions"
  echo "======================"
  echo ""
  for LOG in "${REPORT_DIR}"/logs/*.log; do
    [ -f "${LOG}" ] || continue
    DEFINITELY=$(grep -oP 'definitely lost: \K[0-9,]+' "${LOG}" 2>/dev/null | tr -d ',' || echo "0")
    DEFINITELY=${DEFINITELY:-0}
    [ "${DEFINITELY}" -eq 0 ] && continue
    test_name=$(basename "${LOG}" .log)
    echo "## ${test_name}"
    echo ""
    grep -A2 "definitely lost:" "${LOG}" 2>/dev/null || true
    grep -B1 -A5 "at 0x" "${LOG}" 2>/dev/null | head -20 || true
    echo ""
    echo "Suggestion: Check for missing delete/free calls or use smart pointers."
    echo ""
  done
} > "${SUGGESTIONS_FILE}"

echo "==> Generating HTML report"
cat > "${REPORT_DIR}/memory_${TIMESTAMP}.html" <<HTMLEOF
<!DOCTYPE html>
<html>
<head>
<title>NekoEcat Studio Memory Report</title>
<style>
  body { font-family: system-ui, sans-serif; max-width: 900px; margin: 40px auto; padding: 0 20px; }
  table { border-collapse: collapse; width: 100%; margin: 20px 0; }
  th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }
  th { background: #f5f5f5; }
  .ok { color: #16a34a; font-weight: bold; }
  .leak { color: #dc2626; font-weight: bold; }
  .error { color: #d97706; font-weight: bold; }
  h1 { border-bottom: 2px solid #7c3aed; padding-bottom: 10px; }
  .section { margin: 30px 0; }
  pre { background: #f8f8f8; padding: 15px; overflow-x: auto; }
</style>
</head>
<body>
<h1>NekoEcat Studio Memory Leak Report</h1>
<p>Generated: ${TIMESTAMP}</p>

<div class="section">
<h2>Overview</h2>
<table>
  <tr><th>Metric</th><th>Value</th></tr>
  <tr><td>Total Tests</td><td>${TOTAL_TESTS}</td></tr>
  <tr><td>Clean</td><td class="ok">${CLEAN_TESTS}</td></tr>
  <tr><td>With Leaks</td><td class="leak">${LEAK_TESTS}</td></tr>
  <tr><td>With Errors</td><td class="error">${ERROR_TESTS}</td></tr>
  <tr><td>Total Bytes Lost</td><td>${TOTAL_LOST}</td></tr>
</table>
</div>

<div class="section">
<h2>Details</h2>
<pre>$(cat "${SUMMARY_FILE}")</pre>
</div>

<div class="section">
<h2>Fix Suggestions</h2>
<pre>$(cat "${SUGGESTIONS_FILE}")</pre>
</div>
</body>
</html>
HTMLEOF

ln -sf "${REPORT_DIR}/memory_${TIMESTAMP}.html" "${REPORT_DIR}/latest.html"

echo ""
echo "==> Memory analysis complete"
echo "    Summary:     ${SUMMARY_FILE}"
echo "    Suggestions: ${SUGGESTIONS_FILE}"
echo "    HTML report: ${REPORT_DIR}/memory_${TIMESTAMP}.html"
echo "    Latest:      ${REPORT_DIR}/latest.html"
echo ""
cat "${SUMMARY_FILE}"

rm -rf "${BUILD_DIR}"
