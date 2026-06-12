# src/igh — IgH EtherCAT Master Adapter

Wraps the IgH `ethercat` CLI, parses stdout into structured domain
objects. Keeps all CLI-specific parsing out of the daemon core.

## Files

| File | Purpose |
|------|---------|
| `EthercatCliBackend` | CLI invocation, stdout parsing, error handling |
