#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-coverage"
REPORT_DIR="${ROOT_DIR}/build/reports/coverage"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
TREND_FILE="${REPORT_DIR}/trend.csv"

for arg in "$@"; do
  case "${arg}" in
    --help|-h)
      echo "Usage: $0"
      echo "  Runs tests with coverage instrumentation and generates reports."
      exit 0
      ;;
  esac
done

cd "${ROOT_DIR}"

for tool in lcov genhtml gcovr cmake; do
  if ! command -v "${tool}" &>/dev/null; then
    echo "Error: ${tool} not found. Install with:"
    echo "  sudo apt-get install lcov gcovr cmake"
    exit 1
  fi
done

echo "==> Configuring with coverage flags"
cmake -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"

echo "==> Building"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo "==> Running tests"
ctest --test-dir "${BUILD_DIR}" --output-on-failure -j"$(nproc)" || true

echo "==> Collecting coverage data"
mkdir -p "${REPORT_DIR}"

lcov --capture \
  --directory "${BUILD_DIR}" \
  --output-file "${REPORT_DIR}/coverage_raw.info" \
  --ignore-errors mismatch

lcov --remove "${REPORT_DIR}/coverage_raw.info" \
  '/usr/*' '*/tests/*' '*/moc_*' '*/build*' \
  --output-file "${REPORT_DIR}/coverage.info" \
  --ignore-errors unused

echo "==> Generating HTML report"
genhtml "${REPORT_DIR}/coverage.info" \
  --output-directory "${REPORT_DIR}/html" \
  --title "NekoEcat Studio Coverage Report" \
  --legend --demangle-cpp

echo "==> Generating text summary"
lcov --list "${REPORT_DIR}/coverage.info" > "${REPORT_DIR}/summary.txt"

echo "==> Generating gcovr reports"
gcovr --root "${ROOT_DIR}" \
  --filter 'src/' --filter 'apps/' \
  --exclude '.*tests/.*' \
  --xml "${REPORT_DIR}/coverage.xml" \
  --html-details "${REPORT_DIR}/gcovr/index.html" \
  --print-summary 2>&1 | tee "${REPORT_DIR}/gcovr_summary.txt"

echo "==> Identifying uncovered code"
gcovr --root "${ROOT_DIR}" \
  --filter 'src/' --filter 'apps/' \
  --exclude '.*tests/.*' \
  --fail-under-line 0 \
  --print-summary 2>&1 | \
  grep -E '(TOTAL|[0-9]+\.[0-9]+%)' > "${REPORT_DIR}/coverage_metrics.txt" || true

LINE_PCT=$(grep -oP 'line: \K[0-9.]+' "${REPORT_DIR}/gcovr_summary.txt" 2>/dev/null || echo "0")
BRANCH_PCT=$(grep -oP 'branch: \K[0-9.]+' "${REPORT_DIR}/gcovr_summary.txt" 2>/dev/null || echo "0")

echo "${TIMESTAMP},${LINE_PCT},${BRANCH_PCT}" >> "${TREND_FILE}"
echo "==> Trend updated: ${TIMESTAMP}, line=${LINE_PCT}%, branch=${BRANCH_PCT}%"

echo ""
echo "==> Coverage analysis complete"
echo "    HTML report:   ${REPORT_DIR}/html/index.html"
echo "    gcovr report:  ${REPORT_DIR}/gcovr/index.html"
echo "    Summary:       ${REPORT_DIR}/summary.txt"
echo "    Metrics:       ${REPORT_DIR}/coverage_metrics.txt"
echo "    Trend:         ${REPORT_DIR}/trend.csv"
echo "    XML (CI):      ${REPORT_DIR}/coverage.xml"
echo ""
cat "${REPORT_DIR}/coverage_metrics.txt" 2>/dev/null || true

rm -rf "${BUILD_DIR}"
