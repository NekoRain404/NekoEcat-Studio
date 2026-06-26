# NekoEcat Studio User Manual

## Overview

NekoEcat Studio is a modern EtherCAT engineering workstation for Linux, built on the IgH EtherCAT Master stack. It provides comprehensive tools for EtherCAT commissioning, diagnostics, and monitoring.

## Getting Started

### First Launch

1. Start NekoEcat Studio: `ecat-studio`
2. Select your EtherCAT network adapter from the toolbar
3. Click "Connect" to scan the bus

### Main Interface

The interface is organized into workspaces:

- **Overview**: Bus topology and slave status
- **Object Dictionary**: Browse and edit OD entries
- **SDO/PDO**: SDO read/write and PDO mapping
- **Watch**: Monitor real-time values
- **Startup SDO**: Configure startup SDO sequences
- **Free Run**: Process image I/O testing
- **Diagnostics**: Error analysis and health monitoring
- **I/O Variables**: Variable engineering table

## Core Features

### Topology View

- Visual bus topology with slave status indicators
- Drag-and-drop slave rearrangement
- Real-time state machine monitoring

### Object Dictionary

- Hierarchical OD browser with search
- SDO read/write with evidence tracking
- Bookmark frequently used entries
- Target trail for tracking access history

### Watch Workspace

- Add multiple SDO targets for monitoring
- Batch read/write operations
- Value change highlighting
- Export watch data to CSV

### Free Run Workspace

- Process image I/O testing
- Real-time value visualization
- Oscilloscope-style signal display
- Data logging and export

### Diagnostics

- Error history and correlation
- AL Event monitoring
- DC Sync status and jitter analysis
- Network health dashboard

### Project Management

- Save/load project files (.ecatproj)
- Project notes and metadata
- Session management
- Export/import configurations

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+N | New project |
| Ctrl+O | Open project |
| Ctrl+S | Save project |
| Ctrl+F | Search OD |
| F5 | Refresh topology |
| Ctrl+D | Disconnect |

## Configuration

### Settings Dialog

Access via `Tools > Settings`:

- **Connection**: Adapter selection, timeout settings
- **Display**: Language, theme, font size
- **Logging**: Log level, file output
- **Advanced**: Debug options, performance tuning

### Language Support

NekoEcat Studio supports multiple languages:

- English (default)
- Chinese (Simplified/Traditional)
- Japanese, Korean, German, French, Spanish

## Troubleshooting

### Common Issues

1. **No slaves found**: Check cable connection and master service
2. **SDO timeout**: Increase timeout in settings
3. **Permission error**: Run with appropriate permissions
4. **Display issues**: Check Qt6 theme settings

### Log Files

Logs are stored in `~/.config/NekoEcatStudio/logs/`

## Support

- GitHub Issues: https://github.com/NekoRain404/NekoEcat-Studio
- Documentation: docs/ directory in source package
