# apps/ecatd — Runtime Daemon

Local runtime daemon that listens on `127.0.0.1:5877`. Accepts
newline-delimited JSON commands from the GUI and routes them to
IgH `ethercat` CLI tools or the ecrt Free Run path.

## Files

| File | Purpose |
|------|---------|
| `EcatDaemon` | TCP server, command dispatch, master lifecycle |
| `FreeRunController` | ecrt-based process image I/O |
| `main.cpp` | Entry point |
