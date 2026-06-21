# Advanced Testing & Quality Scripts

This directory contains scripts for advanced testing, benchmarking, and code quality analysis.

## Scripts

### `analyze_coverage.sh`
Runs tests with coverage instrumentation and generates reports.

**Outputs:**
- `build/reports/coverage/html/index.html` — Line-by-line HTML coverage
- `build/reports/coverage/coverage.xml` — Cobertura XML (for CI)
- `build/reports/coverage/trend.csv` — Coverage trends over time
- `build/reports/coverage/summary.txt` — Text summary

**Usage:**
```bash
bash scripts/analyze_coverage.sh
```

### `benchmark.sh`
Runs performance benchmarks for startup, memory, and I/O.

**Options:**
- `--iterations=N` — Number of benchmark iterations (default: 5)

**Outputs:**
- `build/reports/benchmark/latest.html` — HTML report
- `build/reports/benchmark/results_*.json` — Raw JSON data
- `build/reports/benchmark/trend.csv` — Performance trends

**Usage:**
```bash
bash scripts/benchmark.sh --iterations=10
```

### `check_memory.sh`
Runs tests under Valgrind to detect memory leaks.

**Outputs:**
- `build/reports/memory/latest.html` — HTML report
- `build/reports/memory/summary_*.txt` — Leak summary
- `build/reports/memory/suggestions_*.txt` — Fix suggestions

**Usage:**
```bash
bash scripts/check_memory.sh
```

### `check_quality.sh`
Runs static analysis, formatting checks, and complexity analysis.

**Tools used:**
- cppcheck — Static analysis
- clang-tidy — Modern C++ linter
- clang-format — Style checker

**Outputs:**
- `build/reports/quality/latest.html` — HTML report
- `build/reports/quality/trend.csv` — Quality trends

**Usage:**
```bash
bash scripts/check_quality.sh
```

## CI/CD Integration

The `advanced-analysis` job in `.github/workflows/ci.yml` runs automatically on pushes to `main`. It executes:
1. Coverage analysis
2. Memory leak detection
3. Code quality analysis

All reports are uploaded as artifacts with 30-day retention.

## Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install lcov gcovr valgrind cppcheck clang-tidy clang-format

# Optional: complexity analysis
pip install lizard
```
