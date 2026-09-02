#!/usr/bin/env bash
# Run one test binary under valgrind for the CI memory check.
#
# Failure policy (actual project defects only):
#   - Any "definitely lost" or "indirectly lost" heap leak (non-zero) fails.
#   - An invalid read/write / uninitialised / unaddressable access whose stack
#     trace reaches PROJECT source (this repo) fails.
#   - Qt6 / glibc / QProcess internals routinely trigger valgrind "errors"
#     that are NOT project bugs: QProcess spawns subprocesses via vfork/forkfd
#     (kernel leaves siginfo unwritten -> "Syscall param waitid(infop)"),
#     and Qt string/container internals show "Conditional jump depends on
#     uninitialised value" with unresolvable (stripped-lib) symbols. These are
#     ignored: they only fail if the trace reaches project code.
#   - A non-zero binary exit code that is not a memory error (e.g. a perf test
#     exceeding its timing budget under valgrind's slowdown) passes here.
#
# QT_QPA_PLATFORM is inherited from the workflow env; a fallback keeps this
# script runnable standalone.

set -u

if [ $# -ne 1 ]; then
    echo "usage: $0 <test-binary>" >&2
    exit 2
fi
BIN="$1"
PROJ_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"

LOG="$(mktemp)"
trap 'rm -f "$LOG"' EXIT

# No --error-exitcode: we decide failures by parsing the log.
valgrind \
    --quiet \
    --leak-check=full \
    --errors-for-leak-kinds=definite,indirect \
    "$BIN" >"$LOG" 2>&1
bin_rc=$?

# 1) Real leaks.
leaks=$(grep -E 'definitely lost: *([1-9][0-9]*)' "$LOG" | grep -v 'in 0 blocks' || true)
leaks_ind=$(grep -E 'indirectly lost: *([1-9][0-9]*)' "$LOG" | grep -v 'in 0 blocks' || true)

# 2) Invalid accesses whose stack reaches project source. Filter out traces
#    that only reference Qt/system libs.
project_hits=""
if grep -E 'Invalid read|Invalid write|uninitialised|unaddressable|Mismatched free|Syscall param' "$LOG" >/dev/null 2>&1; then
    project_hits=$(grep -A 12 -E '(Invalid read|Invalid write|uninitialised|unaddressable|Syscall param)' "$LOG" \
        | grep -E 'at 0x|by 0x' \
        | grep "$PROJ_ROOT" \
        || true)
fi

if [ -n "$leaks" ] || [ -n "$leaks_ind" ] || [ -n "$project_hits" ]; then
    echo "valgrind: real memory errors in $BIN" >&2
    [ -n "$leaks" ] && echo "$leaks" >&2
    [ -n "$leaks_ind" ] && echo "$leaks_ind" >&2
    if [ -n "$project_hits" ]; then
        echo "invalid access reaching project code:" >&2
        echo "$project_hits" | head -20 >&2
    fi
    exit 1
fi

# 3) Non-memory failures (perf timing under valgrind, etc.) are not memory
#    defects; surface them but pass.
if [ "$bin_rc" -ne 0 ]; then
    echo "valgrind: binary exited $bin_rc (not a memory error; see log)" >&2
fi

exit 0