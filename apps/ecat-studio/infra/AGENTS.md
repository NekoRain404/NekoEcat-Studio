# infra/ — Infrastructure

Core infrastructure shared across the GUI.

## Files

| File | Purpose |
|------|---------|
| `EcatClient` | JSON-over-TCP client for the ecatd daemon |
| `LanguageManager` | Centralized language registry (8 languages) |
| `ProcessDataTypes` | Shared POD types for PDO, Free Run, I/O variables |
| `SettingsDialog` | Comprehensive tabbed settings dialog (7 sections: Appearance, EtherCAT, Timing, Free Run, Display, Notifications, Export) |
| `TranslationRegistry` | Runtime translation lookup; supported languages and entries are defined in code |

## SettingsDialog Sections

The settings dialog is organized into tabs:
- **Appearance**: Theme (Dark/Light), Language (8 languages), UI Scale
- **EtherCAT**: Master profiles (add/remove/rename IgH master selectors)
- **Timing**: Watch/Overview auto-refresh, SDO timeouts, topology polling
- **Free Run**: Cycle time, auto-name, change highlighting
- **Display**: Raw tabs, grid lines, alternating rows, compact mode, detail width, row height, history limit
- **Notifications**: State change, error, watch drift, sound, toast duration
- **Export**: Default directory, ESI path, timestamps, metadata, CSV delimiter
