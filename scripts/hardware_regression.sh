#!/usr/bin/env bash
#
# NekoEcat hardware regression test.
#
# Cycles a selected slave on a LIVE IgH EtherCAT bus through the AL state chain
# INIT -> PREOP -> SAFEOP -> OP, verifying each transition, then reads the
# mandatory identity SDO objects 0x1000:0 (device type) and 0x1018:1
# (vendor ID). Optionally exercises the SDO write path (--allow-write).
#
# The script talks to the bus through the ecatd JSON-RPC daemon
# (127.0.0.1:5877) when available and falls back to the `ethercat` CLI.
#
# DANGER: this tool CHANGES THE STATE OF LIVE SLAVES. Run it only on a bus you
# are allowed to reconfigure. On failure the selected slave is returned to INIT
# (safe state). Use --slave to target one slave; other slaves are not touched.
#
# Environment overrides (documented in docs/DEVELOPER_GUIDE.md):
#   NEKOECAT_MASTER_DEV      master device node        (default /dev/EtherCAT0)
#   NEKOECAT_MASTER          IgH master index          (default 0)
#   NEKOECAT_WRITE_TEST_INDEX/_SUBINDEX/_TYPE  SDO used by --allow-write
#                           (default 0x1018:1 uint32; may be read-only on some
#                            slaves - override for a writable object)
#   NEKOECAT_NO_SUDO=1       never sudo-wrap the CLI
#   NEKOECAT_PYTHON          python interpreter        (default python3)
set -euo pipefail

# ------------------------------------------------------------------ config
SLAVE_POS=0
TARGET_STATE=OP
DAEMON_HOST=127.0.0.1
DAEMON_PORT=5877
FORCE_DAEMON=0
VERBOSE=0
ETHERCAT_BIN=""
ALLOW_WRITE=0

MASTER_DEV="${NEKOECAT_MASTER_DEV:-/dev/EtherCAT0}"
MASTER_IDX="${NEKOECAT_MASTER:-0}"
PYTHON="${NEKOECAT_PYTHON:-python3}"

SDO_DEVICE_TYPE_INDEX=0x1000
SDO_DEVICE_TYPE_SUB=0
SDO_VENDOR_INDEX=0x1018
SDO_VENDOR_SUB=1

WRITE_TEST_INDEX="${NEKOECAT_WRITE_TEST_INDEX:-0x1018}"
WRITE_TEST_SUB="${NEKOECAT_WRITE_TEST_SUBINDEX:-1}"
WRITE_TEST_TYPE="${NEKOECAT_WRITE_TEST_TYPE:-uint32}"

STATE_CHAIN=(INIT PREOP SAFEOP OP)
STATE_TIMEOUT_TRIES=50
STATE_POLL_DELAY=0.2

BACKEND=cli
RPC_ID=0
RPC_HELPER=""
RPC_RESULT=""
RPC_ERROR=""
SUDO=()
if [ "$(id -u)" -ne 0 ] && [ "${NEKOECAT_NO_SUDO:-0}" != "1" ]; then
  SUDO=(sudo)
fi

# ---------------------------------------------------------------- utilities
log() { echo "[hardware-regression] $*" >&2; }
vlog() { [ "$VERBOSE" -eq 1 ] && log "$*"; }
die() { echo "ERROR: $*" >&2; exit 1; }

usage() {
  cat <<'EOF'
Usage: hardware_regression.sh [options]

Cycles a slave through INIT -> PREOP -> SAFEOP -> OP on a live bus, verifying
each transition and reading 0x1000:0 / 0x1018:1. Talks to ecatd over JSON-RPC
when reachable, else falls back to the `ethercat` CLI.

Options:
  --slave N           slave bus position to test        (default 0)
  --target-state S    final state: INIT|PREOP|SAFEOP|OP (default OP)
  --daemon-host H     ecatd RPC host                    (default 127.0.0.1)
  --daemon-port P     ecatd RPC port                    (default 5877)
  --use-daemon        require daemon RPC; fail if ecatd is unreachable
  --ethercat-bin PATH explicit path to the ethercat CLI
  --allow-write       also run the SDO read-modify-write test
  --verbose           verbose logging
  -h, --help          show this help

Environment overrides:
  NEKOECAT_MASTER_DEV        (default /dev/EtherCAT0)
  NEKOECAT_MASTER            (default 0)
  NEKOECAT_WRITE_TEST_INDEX / _SUBINDEX / _TYPE
  NEKOECAT_NO_SUDO=1, NEKOECAT_PYTHON
EOF
  exit 0
}

# ------------------------------------------------------------------ guards
guard_hardware() {
  if [ ! -e "${MASTER_DEV}" ]; then
    echo "ERROR: EtherCAT master device '${MASTER_DEV}' not found." >&2
    echo "       No IgH master/hardware is available on this host." >&2
    echo "       Start the master first, e.g.:  sudo systemctl restart ethercat" >&2
    echo "       Then confirm the node exists:  ls -l ${MASTER_DEV}" >&2
    exit 1
  fi
}

resolve_ethercat() {
  if [ -n "$ETHERCAT_BIN" ]; then
    [ -x "$ETHERCAT_BIN" ] || die "ethercat binary is not executable: ${ETHERCAT_BIN}"
    ETHERCAT_CMD="$ETHERCAT_BIN"
  elif command -v ethercat >/dev/null 2>&1; then
    ETHERCAT_CMD="ethercat"
  else
    die "'ethercat' CLI not found in PATH and no --ethercat-bin given; install IgH (sudo pacman -S ethercat) or point --ethercat-bin at the tool."
  fi
}

# ------------------------------------------------------------------- CLI
cli_run() {
  local -a cmd=()
  if [ ${#SUDO[@]} -gt 0 ]; then cmd+=("${SUDO[@]}"); fi
  cmd+=("$ETHERCAT_CMD")
  if [ -n "$MASTER_IDX" ]; then cmd+=(-m "$MASTER_IDX"); fi
  cmd+=("$@")
  vlog "CLI: ${cmd[*]}"
  "${cmd[@]}"
}

# ----------------------------------------------------------- daemon RPC
write_rpc_helper() {
  RPC_HELPER="$(mktemp "${TMPDIR:-/tmp}/nekoecat-rpc.XXXXXX")"
  cat >"$RPC_HELPER" <<'PY'
import json, os, socket, sys

host = os.environ.get("NEKOECAT_RPC_HOST", "127.0.0.1")
port = int(os.environ.get("NEKOECAT_RPC_PORT", "5877"))

def handle(line):
    line = line.strip()
    if not line:
        return True
    try:
        req = json.loads(line)
    except Exception as exc:
        sys.stdout.write("SYSERR\tbad request json: %r\n" % (exc,))
        return False
    try:
        with socket.create_connection((host, port), timeout=5) as s:
            s.sendall((json.dumps(req, separators=(",", ":")) + "\n").encode("utf-8"))
            frame = b""
            while not frame.endswith(b"\n"):
                chunk = s.recv(65536)
                if not chunk:
                    sys.stdout.write("SYSERR\tconnection closed mid-response\n")
                    return False
                frame += chunk
            resp = json.loads(frame.decode("utf-8"))
    except Exception as exc:
        sys.stdout.write("SYSERR\t%s\n" % (exc,))
        return False
    if resp.get("ok"):
        sys.stdout.write("OK\t%s\n" % json.dumps(resp.get("result", {}), separators=(",", ":")))
    else:
        err = resp.get("error") or {}
        sys.stdout.write("RPCERR\t%s\n" % (err.get("message", "unknown RPC error"),))
    return True

ok = True
for raw in sys.stdin:
    if not handle(raw):
        ok = False
sys.exit(0 if ok else 1)
PY
}

# daemon_call METHOD PARAMS_JSON
#   returns 0 -> OK, result JSON in RPC_RESULT
#   returns 1 -> daemon answered but RPC failed, message in RPC_ERROR
#   returns 2 -> daemon unusable (connection/parse failure)
daemon_call() {
  local method="$1"
  # NB: the obvious default ${2:-{}} is a bash trap — a bare "}" terminates the
  # parameter expansion, appending a literal "}" to every non-empty $2 (params
  # became '{"master":"0"}}'). Use an empty-string default and assign '{}' only
  # when the field is unset/empty, which also keeps set -u happy for 1-arg calls.
  local params="${2:-}"
  [ -n "$params" ] || params='{}'
  RPC_ID=$((RPC_ID + 1))
  RPC_RESULT=""
  RPC_ERROR=""
  req=$(printf '{"id":"hwreg_%d","method":"%s","params":%s}' "$RPC_ID" "$method" "$params")
  vlog "RPC: ${method} ${params}"
  if ! reply=$(printf '%s\n' "$req" | NEKOECAT_RPC_HOST="$DAEMON_HOST" \
                                       NEKOECAT_RPC_PORT="$DAEMON_PORT" \
                                       "$PYTHON" "$RPC_HELPER" 2>/dev/null); then
    return 2
  fi
  case "$reply" in
    OK*)
      RPC_RESULT=$(printf '%s\n' "$reply" | cut -f2-)
      return 0 ;;
    RPCERR*)
      RPC_ERROR=$(printf '%s\n' "$reply" | cut -f2-)
      return 1 ;;
    *)
      return 2 ;;
  esac
}

cleanup() {
  local rc=$?
  if [ "$rc" -ne 0 ] && [ -n "${BACKEND:-}" ]; then
    log "Restoring slave ${SLAVE_POS} to INIT (safe state) after failure"
    if [ "$BACKEND" = rpc ]; then
      daemon_call setState "{\"master\":\"$MASTER_IDX\",\"position\":$SLAVE_POS,\"state\":\"INIT\"}" >/dev/null 2>&1 || true
    else
      cli_run states -p "$SLAVE_POS" INIT >/dev/null 2>&1 || true
    fi
  fi
  if [ -n "$RPC_HELPER" ] && [ -f "$RPC_HELPER" ]; then
    rm -f "$RPC_HELPER"
  fi
}

choose_backend() {
  write_rpc_helper
  trap cleanup EXIT
  if [ "$FORCE_DAEMON" -eq 1 ]; then
    if ! command -v "$PYTHON" >/dev/null 2>&1; then
      die "python3 ('${PYTHON}') not found; required for --use-daemon RPC mode"
    fi
    BACKEND=rpc
    if ! daemon_call ping >/dev/null; then
      die "ecatd not reachable at ${DAEMON_HOST}:${DAEMON_PORT} (--use-daemon given); start it: sudo systemctl start ecatd"
    fi
    log "Backend: ecatd RPC (${DAEMON_HOST}:${DAEMON_PORT})"
    return
  fi
  if command -v "$PYTHON" >/dev/null 2>&1 && daemon_call ping >/dev/null; then
    BACKEND=rpc
    log "Backend: ecatd RPC (${DAEMON_HOST}:${DAEMON_PORT})"
    return
  fi
  BACKEND=cli
  log "Backend: ethercat CLI (daemon RPC unavailable on ${DAEMON_HOST}:${DAEMON_PORT})"
}

# ------------------------------------------------------------- operations
set_state() {
  local st="$1"
  if [ "$BACKEND" = rpc ]; then
    daemon_call setState "{\"master\":\"$MASTER_IDX\",\"position\":$SLAVE_POS,\"state\":\"$st\"}"
    case $? in
      0) return 0 ;;
      1) die "setState ${st} failed on slave ${SLAVE_POS}: ${RPC_ERROR}" ;;
      *) die "daemon connection lost while requesting ${st}" ;;
    esac
  fi
  cli_run states -p "$SLAVE_POS" "$st" >/dev/null \
    || die "ethercat states -p ${SLAVE_POS} ${st} failed"
}

current_state() {
  if [ "$BACKEND" = rpc ]; then
    if ! daemon_call scan "{\"master\":\"$MASTER_IDX\"}"; then
      die "scan failed: ${RPC_ERROR:-daemon unreachable}"
    fi
    "$PYTHON" -c '
import json, sys
d = json.load(sys.stdin)
pos = int(sys.argv[1])
for s in d.get("slaves", []):
    if int(s.get("position", -1)) == pos:
        print(s.get("state", ""))
        break
' <<<"$RPC_RESULT" "$SLAVE_POS"
    return
  fi
  cli_run slaves 2>/dev/null | awk -v p="$SLAVE_POS" '$1==p {print $3; exit}'
}

slave_count() {
  if [ "$BACKEND" = rpc ]; then
    if ! daemon_call scan "{\"master\":\"$MASTER_IDX\"}"; then
      die "scan failed: ${RPC_ERROR:-daemon unreachable}"
    fi
    "$PYTHON" -c 'import json,sys; print(len(json.load(sys.stdin).get("slaves", [])))' <<<"$RPC_RESULT"
    return
  fi
  cli_run slaves 2>/dev/null \
    | awk '/^[[:space:]]*[0-9]+[[:space:]]+[^[:space:]]+/ {n++} END {print n+0}'
}

wait_state() {
  local want="$1" got="" tries=0
  while [ "$tries" -lt "$STATE_TIMEOUT_TRIES" ]; do
    got=$(current_state) || die "failed to read state of slave ${SLAVE_POS}"
    if [ "$got" = "$want" ]; then
      return 0
    fi
    tries=$((tries + 1))
    sleep "$STATE_POLL_DELAY"
  done
  log "Timed out waiting for slave ${SLAVE_POS} to reach ${want} (last seen: '${got}')"
  return 1
}

sdo_upload() {
  local index="$1" sub="$2" type="$3"
  if [ "$BACKEND" = rpc ]; then
    if ! daemon_call upload "{\"master\":\"$MASTER_IDX\",\"position\":$SLAVE_POS,\"index\":\"$index\",\"subIndex\":\"$sub\",\"type\":\"$type\"}"; then
      die "SDO upload ${index}:${sub} failed: ${RPC_ERROR:-daemon unreachable}"
    fi
    "$PYTHON" -c 'import json,sys; print(json.load(sys.stdin).get("value",""))' <<<"$RPC_RESULT"
    return
  fi
  cli_run upload -p "$SLAVE_POS" -t "$type" "$index" "$sub" 2>/dev/null \
    || die "SDO upload ${index}:${sub} failed"
}

sdo_download() {
  local index="$1" sub="$2" type="$3" value="$4"
  if [ "$BACKEND" = rpc ]; then
    daemon_call download "{\"master\":\"$MASTER_IDX\",\"position\":$SLAVE_POS,\"index\":\"$index\",\"subIndex\":\"$sub\",\"value\":\"$value\",\"type\":\"$type\"}"
    case $? in
      0) return 0 ;;
      1) die "SDO download ${index}:${sub} rejected by slave: ${RPC_ERROR}" ;;
      *) die "daemon connection lost during SDO download" ;;
    esac
  fi
  cli_run download -p "$SLAVE_POS" -t "$type" "$index" "$sub" "$value" >/dev/null 2>&1 \
    || die "SDO download ${index}:${sub} rejected by slave (read-only object?)"
}

write_test() {
  local wv="$1" rv=""
  log "Write test (--allow-write): read-modify-write ${WRITE_TEST_INDEX}:${WRITE_TEST_SUB} (type ${WRITE_TEST_TYPE}) on slave ${SLAVE_POS}"
  sdo_download "$WRITE_TEST_INDEX" "$WRITE_TEST_SUB" "$WRITE_TEST_TYPE" "$wv"
  rv=$(sdo_upload "$WRITE_TEST_INDEX" "$WRITE_TEST_SUB" "$WRITE_TEST_TYPE")
  if [ -z "$rv" ]; then
    die "write test: re-read of ${WRITE_TEST_INDEX}:${WRITE_TEST_SUB} came back empty"
  fi
  if [ "$rv" != "$wv" ]; then
    die "write test: read-back mismatch ${rv} != ${wv} for ${WRITE_TEST_INDEX}:${WRITE_TEST_SUB}"
  fi
  log "Write test OK: ${WRITE_TEST_INDEX}:${WRITE_TEST_SUB} = ${rv}"
}

# ------------------------------------------------------------------- main
main() {
  guard_hardware
  resolve_ethercat
  choose_backend

  local n dt vd
  n=$(slave_count) || die "unable to count slaves"
  if [ "$n" -lt 1 ]; then
    die "no slaves detected on the bus (master ${MASTER_IDX}); nothing to regression-test"
  fi
  log "Slaves on bus: ${n}"
  if [ "$SLAVE_POS" -ge "$n" ]; then
    die "slave position ${SLAVE_POS} out of range (${n} slave(s) on bus)"
  fi

  set_state INIT
  wait_state INIT || die "slave ${SLAVE_POS} did not reach INIT"
  log "State OK: INIT"

  if [ "$TARGET_STATE" != "INIT" ]; then
    local st
    for st in PREOP SAFEOP OP; do
      set_state "$st"
      wait_state "$st" || die "slave ${SLAVE_POS} failed to reach ${st}"
      log "State OK: ${st}"
      [ "$st" = "$TARGET_STATE" ] && break
    done
  fi

  dt=$(sdo_upload "$SDO_DEVICE_TYPE_INDEX" "$SDO_DEVICE_TYPE_SUB" uint32)
  [ -n "$dt" ] || die "SDO ${SDO_DEVICE_TYPE_INDEX}:${SDO_DEVICE_TYPE_SUB} (device type) read back empty"
  log "Device type  ${SDO_DEVICE_TYPE_INDEX}:${SDO_DEVICE_TYPE_SUB} = ${dt}"

  vd=$(sdo_upload "$SDO_VENDOR_INDEX" "$SDO_VENDOR_SUB" uint32)
  [ -n "$vd" ] || die "SDO ${SDO_VENDOR_INDEX}:${SDO_VENDOR_SUB} (vendor ID) read back empty"
  log "Vendor ID    ${SDO_VENDOR_INDEX}:${SDO_VENDOR_SUB} = ${vd}"

  if [ "$ALLOW_WRITE" -eq 1 ]; then
    write_test "$vd"
  fi

  log "PASS: slave ${SLAVE_POS} reached ${TARGET_STATE}; identity SDOs verified"
}

# --------------------------------------------------------------- options
while [ $# -gt 0 ]; do
  case "$1" in
    --slave) SLAVE_POS="${2:?--slave needs a value}"; shift 2 ;;
    --target-state) TARGET_STATE="${2:?--target-state needs a value}"; shift 2 ;;
    --daemon-host) DAEMON_HOST="${2:?--daemon-host needs a value}"; shift 2 ;;
    --daemon-port) DAEMON_PORT="${2:?--daemon-port needs a value}"; shift 2 ;;
    --use-daemon) FORCE_DAEMON=1; shift ;;
    --ethercat-bin) ETHERCAT_BIN="${2:?--ethercat-bin needs a value}"; shift 2 ;;
    --allow-write) ALLOW_WRITE=1; shift ;;
    --verbose) VERBOSE=1; shift ;;
    -h|--help) usage ;;
    *) die "unknown option: $1 (see --help)" ;;
  esac
done

case "$SLAVE_POS" in
  ''|*[!0-9]*) die "--slave must be a non-negative integer, got: ${SLAVE_POS}" ;;
esac
case "$TARGET_STATE" in
  ''|*[!A-Za-z]*) die "--target-state must be one of: ${STATE_CHAIN[*]}" ;;
esac
TARGET_STATE=$(printf '%s' "$TARGET_STATE" | tr '[:lower:]' '[:upper:]')
case "$TARGET_STATE" in
  INIT|PREOP|SAFEOP|OP) ;;
  *) die "--target-state must be one of: ${STATE_CHAIN[*]}" ;;
esac

main
