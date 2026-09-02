# ecatd systemd service

This directory ships the systemd unit that runs the NekoEcat runtime daemon
(`ecatd`) as a boot-time service. It is included in the Linux binary tarball
under `packaging/systemd/`.

`ecatd` is a local IPC service: it binds to `127.0.0.1:5877`, accepts
newline-delimited JSON-RPC, and bridges to the IgH EtherCAT master. Running it
under systemd gives you automatic start on boot, supervision (restart on
failure), journal logging, and a hardened resource envelope (RT priority +
memory lock).

## Files

| File              | Purpose                                              |
|-------------------|------------------------------------------------------|
| `ecatd.service`   | systemd unit for the runtime daemon                  |
| `README.md`       | This document                                        |

## Install

```bash
sudo cp packaging/systemd/ecatd.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now ecatd
```

The unit declares `Wants=ethercat.service` and `After=network.target
ethercat.service`, so it waits for the IgH master to be up before starting.
If the master is missing, ecatd is still started (Wants, not Requires) and
`Restart=on-failure` keeps it supervised.

## Verify

```bash
systemctl status ecatd
journalctl -u ecatd -f          # live logs
journalctl -u ecatd             # recent logs
systemctl is-enabled ecatd
```

Expected first log line when healthy:

```
ecatd listening on 127.0.0.1:5877
```

## Uninstall

```bash
sudo systemctl disable --now ecatd
sudo rm /etc/systemd/system/ecatd.service
sudo systemctl daemon-reload
```

## Configuration

### Firmware directory

FoE firmware transfers are confined to `NEKOECAT_FIRMWARE_DIR`. The unit sets it
to `/var/lib/nekoecat/firmware`. Create it before first use:

```bash
sudo install -d -o root -g root /var/lib/nekoecat/firmware
```

Override it with a drop-in if you keep firmware elsewhere:

```bash
sudo systemctl edit ecatd
```

```ini
[Service]
Environment=NEKOECAT_FIRMWARE_DIR=/srv/nekoecat/firmware
```

### Running from a build tree (drop-in)

The unit's `ExecStart` points at the packaged `/usr/local/bin/ecatd`. To use a
build-tree binary without editing the shipped unit:

```bash
sudo systemctl edit ecatd
```

```ini
[Service]
ExecStart=/home/user/nekoecat/build/apps/ecatd/ecatd --foreground
```

Then:

```bash
sudo systemctl daemon-reload && sudo systemctl restart ecatd
```

> Note: systemd applies all `ExecStart=` overrides in a drop-in, not just the
> first one; use a fresh `[Service]` block and only one `ExecStart=` line.

## Hardening

The unit runs `ecatd` as root because the daemon currently has no seccomp /
landlock profile and needs raw access to `/dev/EtherCAT0` plus RT scheduling
for the Free Run loop. This is appropriate for a commissioning workstation but
not for a hardened production host.

To run it as an unprivileged system user with only the needed capabilities,
start with the drop-in shown in the `ecatd.service` header comment:

```bash
sudo useradd --system --home-dir /var/lib/nekoecat --create-home ecatd
sudo install -d -o ecatd -g ecatd /var/lib/nekoecat/firmware
sudo systemctl edit ecatd
```

```ini
[Service]
User=ecatd
Group=ecatd
AmbientCapabilities=CAP_SYS_NICE CAP_IPC_LOCK
CapabilityBoundingSet=CAP_SYS_NICE CAP_IPC_LOCK
DeviceAllow=/dev/EtherCAT0 rw
NoNewPrivileges=true
ProtectSystem=strict
ProtectHome=true
PrivateTmp=true
```

```bash
sudo systemctl daemon-reload && sudo systemctl restart ecatd
```

Check that the service still reaches the bus and keeps RT scheduling:

```bash
journalctl -u ecatd
systemctl show ecatd -p User,EffectiveCPUsAffinity
```

## Client access

Clients (the GUI, `nekoecat_client_example.py`, or `scripts/hardware_regression.sh`
in `--use-daemon` mode) talk to the daemon over `127.0.0.1:5877`. No special
permissions are needed on the client side.
