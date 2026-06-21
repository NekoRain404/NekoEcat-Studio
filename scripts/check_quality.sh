#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-quality"
REPORT_DIR="${ROOT_DIR}/build/reports/quality"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

for arg in "$@"; do
  case "${arg}" in
    --help|-h)
      echo "Usage: $0"
      echo "  Runs static analysis, formatting checks, and complexity analysis."
      exit 0
      ;;
  esac
done

cd "${ROOT_DIR}"
mkdir -p "${REPORT_DIR}"

echo "==> Configuring (Debug, compile_commands)"
cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

ISSUES=0

echo "==> Running cppcheck"
CPPCHECK_REPORT="${REPORT_DIR}/cppcheck_${TIMESTAMP}.txt"
cppcheck --enable=warning,style,performance,portability \
  --suppress=missingIncludeSystem \
  --suppress=unmatchedSuppression \
  --error-exitcode=0 \
  --xml --xml-version=2 \
  -I src -I apps \
  src/ apps/ 2>"${REPORT_DIR}/cppcheck_${TIMESTAMP}.xml" || true

cppcheck --enable=warning,style,performance,portability \
  --suppress=missingIncludeSystem \
  --suppress=unmatchedSuppression \
  --error-exitcode=0 \
  -I src -I apps \
  src/ apps/ 2>&1 | tee "${CPPCHECK_REPORT}" || true

CPPCHECK_ERRORS=$(grep -c "error:" "${CPPCHECK_REPORT}" 2>/dev/null || echo "0")
CPPCHECK_WARNINGS=$(grep -c "warning:" "${CPPCHECK_REPORT}" 2>/dev/null || echo "0")
CPPCHECK_STYLE=$(grep -c "style:" "${CPPCHECK_REPORT}" 2>/dev/null || echo "0")
echo "  cppcheck: ${CPPCHECK_ERRORS} errors, ${CPPCHECK_WARNINGS} warnings, ${CPPCHECK_STYLE} style issues"
ISSUES=$((ISSUES + CPPCHECK_ERRORS + CPPCHECK_WARNINGS))

echo "==> Running clang-tidy"
CLANGTIDY_REPORT="${REPORT_DIR}/clang-tidy_${TIMESTAMP}.txt"
if command -v clang-tidy &>/dev/null; then
  find src apps -name '*.cpp' | head -100 | \
    xargs -r clang-tidy -p "${BUILD_DIR}" --quiet 2>&1 | \
    tee "${CLANGTIDY_REPORT}" || true
  CLANGTIDY_WARNINGS=$(grep -c "warning:" "${CLANGTIDY_REPORT}" 2>/dev/null || echo "0")
  echo "  clang-tidy: ${CLANGTIDY_WARNINGS} warnings"
  ISSUES=$((ISSUES + CLANGTIDY_WARNINGS))
else
  echo "  clang-tidy: not found, skipping"
  echo "Not available" > "${CLANGTIDY_REPORT}"
  CLANGTIDY_WARNINGS=0
fi

echo "==> Checking code formatting"
FORMAT_REPORT="${REPORT_DIR}/format_${TIMESTAMP}.txt"
FORMAT_VIOLATIONS=0
if command -v clang-format &>/dev/null; then
  find src apps tests -name '*.cpp' -o -name '*.h' | \
    xargs -r clang-format --dry-run --Werror 2>&1 | \
    tee "${FORMAT_REPORT}" || FORMAT_VIOLATIONS=$(grep -c "warning:" "${FORMAT_REPORT}" 2>/dev/null || echo "0")
  FORMAT_VIOLATIONS=${FORMAT_VIOLATIONS:-0}
  echo "  formatting: ${FORMAT_VIOLATIONS} violations"
  ISSUES=$((ISSUES + FORMAT_VIOLATIONS))
else
  echo "  clang-format: not found, skipping"
  echo "Not available" > "${FORMAT_REPORT}"
fi

echo "==> Analyzing code complexity"
COMPLEXITY_REPORT="${REPORT_DIR}/complexity_${TIMESTAMP}.txt"
if command -v lizard &>/dev/null; then
  lizard src/ apps/ \
    --length 100 \
    --CCN 15 \
    --arguments 10 \
    -w 2>&1 | tee "${COMPLEXITY_REPORT}" || true
  COMPLEX_FUNCTIONS=$(grep -c "warning:" "${COMPLEXITY_REPORT}" 2>/dev/null || echo "0")
  echo "  complexity: ${COMPLEX_FUNCTIONS} high-complexity functions"
else
  echo "  lizard not found, using basic analysis"
  {
    echo "Function complexity (approximate)"
    echo "================================="
    for dir in src apps; do
      find "${dir}" -name '*.cpp' -o -name '*.h' | while read -r f; do
        awk '/^[a-zA-Z].*\(/ && !/;/ && !/\/\// { count++ }
             /^[{}]/ { if(count>0) depth++ }
             END { if(count>0) print FILENAME": "count" functions" }' "${f}" 2>/dev/null || true
      done
    done
  } > "${COMPLEXITY_REPORT}"
  COMPLEX_FUNCTIONS=0
fi

echo "==> Detecting code duplication"
DUP_REPORT="${REPORT_DIR}/duplication_${TIMESTAMP}.txt"
if command -v pmccabe &>/dev/null; then
  find src apps -name '*.cpp' | xargs -r pmccabe 2>/dev/null | \
    sort -rn | head -20 > "${DUP_REPORT}" || true
else
  {
    echo "Duplicate code detection (basic)"
    echo "================================="
    echo "For detailed analysis, install: pip install lizard"
    echo ""
    echo "Files with identical includes (>5 lines):"
    for dir in src apps; do
      find "${dir}" -name '*.cpp' | while read -r f; do
        head -20 "${f}" 2>/dev/null | grep '#include' | sort
      done | sort | uniq -d | head -10 || true
    done
  } > "${DUP_REPORT}"
fi
echo "  duplication: report generated"

echo "==> Generating trend data"
TREND_FILE="${REPORT_DIR}/trend.csv"
if [ ! -f "${TREND_FILE}" ]; then
  echo "timestamp,cppcheck_issues,clang_tidy_warnings,format_violations" > "${TREND_FILE}"
fi
echo "${TIMESTAMP},${CPPCHECK_ERRORS},${CLANGTIDY_WARNINGS},${FORMAT_VIOLATIONS}" >> "${TREND_FILE}"

echo "==> Generating HTML report"
cat > "${REPORT_DIR}/quality_${TIMESTAMP}.html" <<HTMLEOF
<!DOCTYPE html>
<html>
<head>
<title>NekoEcat Studio Code Quality Report</title>
<style>
  body { font-family: system-ui, sans-serif; max-width: 900px; margin: 40px auto; padding: 0 20px; }
  table { border-collapse: collapse; width: 100%; margin: 20px 0; }
  th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }
  th { background: #f5f5f5; }
  h1 { border-bottom: 2px solid #059669; padding-bottom: 10px; }
  .section { margin: 30px 0; }
  pre { background: #f8f8f8; padding: 15px; overflow-x: auto; max-height: 400px; overflow-y: auto; }
  .good { color: #16a34a; font-weight: bold; }
  .warn { color: #d97706; font-weight: bold; }
  .bad { color: #dc2626; font-weight: bold; }
</style>
</head>
<body>
<h1>NekoEcat Studio Code Quality Report</h1>
<p>Generated: ${TIMESTAMP}</p>

<div class="section">
<h2>Summary</h2>
<table>
  <tr><th>Check</th><th>Issues</th><th>Status</th></tr>
  <tr>
    <td>cppcheck</td>
    <td>${CPPCHECK_ERRORS} errors, ${CPPCHECK_WARNINGS} warnings, ${CPPCHECK_STYLE} style</td>
    <td class="$([ "${CPPCHECK_ERRORS}" -gt 0 ] && echo 'bad' || ([ "${CPPCHECK_WARNINGS}" -gt 0 ] && echo 'warn' || echo 'good'))">$([ "${CPPCHECK_ERRORS}" -gt 0 ] && echo 'FAIL' || ([ "${CPPCHECK_WARNINGS}" -gt 0 ] && echo 'WARN' || echo 'PASS'))</td>
  </tr>
  <tr>
    <td>clang-tidy</td>
    <td>${CLANGTIDY_WARNINGS} warnings</td>
    <td class="$([ "${CLANGTIDY_WARNINGS}" -gt 0 ] && echo 'warn' || echo 'good')">$([ "${CLANGTIDY_WARNINGS}" -gt 0 ] && echo 'WARN' || echo 'PASS')</td>
  </tr>
  <tr>
    <td>Formatting</td>
    <td>${FORMAT_VIOLATIONS} violations</td>
    <td class="$([ "${FORMAT_VIOLATIONS}" -gt 0 ] && echo 'warn' || echo 'good')">$([ "${FORMAT_VIOLATIONS}" -gt 0 ] && echo 'WARN' || echo 'PASS')</td>
  </tr>
</table>
</div>

<div class="section">
<h2>cppcheck Details</h2>
<pre>$(head -200 "${CPPCHECK_REPORT}" 2>/dev/null || echo "No data")</pre>
</div>

<div class="section">
<h2>clang-tidy Details</h2>
<pre>$(head -200 "${CLANGTIDY_REPORT}" 2>/dev/null || echo "No data")</pre>
</div>

<div class="section">
<h2>Formatting Issues</h2>
<pre>$(head -100 "${FORMAT_REPORT}" 2>/dev/null || echo "No data")</pre>
</div>

<div class="section">
<h2>Complexity Analysis</h2>
<pre>$(head -100 "${COMPLEXITY_REPORT}" 2>/dev/null || echo "No data")</pre>
</div>

<div class="section">
<h2>Duplication Analysis</h2>
<pre>$(cat "${DUP_REPORT}" 2>/dev/null || echo "No data")</pre>
</div>

<div class="section">
<h2>Trend</h2>
<pre>$(cat "${TREND_FILE}" 2>/dev/null || echo "No trend data yet")</pre>
</div>
</body>
</html>
HTMLEOF

ln -sf "${REPORT_DIR}/quality_${TIMESTAMP}.html" "${REPORT_DIR}/latest.html"

echo ""
echo "==> Quality analysis complete"
echo "    Total issues: ${ISSUES}"
echo "    HTML report:  ${REPORT_DIR}/quality_${TIMESTAMP}.html"
echo "    Latest:       ${REPORT_DIR}/latest.html"
echo "    Trend:        ${REPORT_DIR}/trend.csv"
echo ""
echo "    cppcheck:   ${CPPCHECK_REPORT}"
echo "    clang-tidy: ${CLANGTIDY_REPORT}"
echo "    format:     ${FORMAT_REPORT}"
echo "    complexity: ${COMPLEXITY_REPORT}"
echo "    duplication:${DUP_REPORT}"

rm -rf "${BUILD_DIR}"
