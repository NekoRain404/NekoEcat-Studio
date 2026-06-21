#pragma once

/// @brief Workspace plugin for real-time multi-channel waveform display.
///
/// @details The Oscilloscope workspace provides a multi-channel oscilloscope
/// interface for real-time waveform visualization. It supports configurable
/// timebase, trigger modes, cursor measurements, and channel management.
///
/// Features:
///   - **Multi-channel oscilloscope**: Display multiple signal channels simultaneously
///   - **Configurable timebase**: Adjustable time/division for waveform scaling
///   - **Trigger modes**: Auto, Normal, and Single trigger modes
///   - **Trigger level adjustment**: Spin box for precise trigger level control
///   - **Cursor measurements**: Crosshair cursors with readout display
///   - **Channel management**: Add/remove channels via dialog
///   - **Run/Stop acquisition**: Toggle waveform acquisition
///   - **Real-time refresh**: Continuous waveform updates
///
/// @par Constructor
///   OscilloscopePlugin(OscilloscopeService *service, QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "oscilloscope"
///   - defaultOrder: 100
///   - visible: always true
///
/// @par UI Description
///   The oscilloscope workspace displays a custom OscilloscopeWidget for waveform
///   rendering. A channel list panel on the left shows active channels. Controls
///   at the top include timebase selector, trigger mode/level, run/stop button,
///   and cursor toggle. The cursor readout label shows measurement values.
///
/// @par Usage Example
///   @code
///   // In MainWindow constructor:
///   auto *oscService = new OscilloscopeService(this);
///   pluginRegistry_->registerPlugin(new OscilloscopePlugin(oscService, this));
///
///   // Add a channel:
///   oscPlugin->showAddChannelDialog();
///
///   // Start acquisition:
///   oscPlugin->toggleAcquisition();
///   @endcode
///
/// @see WorkspacePlugin, OscilloscopeService, OscilloscopeWidget

#include "plugins/WorkspacePlugin.h"

class QListWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QPushButton;
class OscilloscopeWidget;
class OscilloscopeService;

/// @brief Workspace plugin for real-time multi-channel waveform display.
///
/// @details This plugin provides a complete oscilloscope interface with:
///   - Multi-channel waveform visualization using OscilloscopeWidget
///   - Channel list management with add/remove functionality
///   - Timebase and trigger configuration controls
///   - Cursor measurement with readout display
///   - Run/Stop acquisition control
///
/// The plugin communicates with OscilloscopeService for signal data and uses
/// QTimer for periodic waveform refresh.
class OscilloscopePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  /// @brief Constructs the Oscilloscope plugin with fine-grained service injection.
  /// @param service  OscilloscopeService instance for signal data management
  /// @param parent   Qt parent object (typically MainWindow)
  explicit OscilloscopePlugin(OscilloscopeService *service,
                               QObject *parent = nullptr);

  // ── WorkspacePlugin Identity ──────────────────────────────────
  QString id() const override;           ///< Returns "oscilloscope"
  QString displayName() const override;  ///< Returns "Oscilloscope"
  QString displayNameZh() const override; ///< Returns "示波器"
  QWidget *widget() override;            ///< Returns the root container widget
  int defaultOrder() const override;     ///< Returns 100
  bool visible() const override;         ///< Returns true (always visible)

  // ── Accessors ─────────────────────────────────────────────────
  OscilloscopeService *service() const { return service_; } ///< Returns the Oscilloscope service instance

private slots:
  /// @brief Opens a dialog to add a new signal channel.
  /// @details Shows a list of available signal sources and allows the user
  /// to select one or more channels to add to the oscilloscope display.
  void showAddChannelDialog();

  /// @brief Removes the selected channel from the oscilloscope.
  void removeSelectedChannel();

  /// @brief Toggles waveform acquisition between Run and Stop states.
  void toggleAcquisition();

  /// @brief Refreshes the waveform display with current signal data.
  /// @details Called periodically by the internal timer to update the display.
  void refreshWaveforms();

  /// @brief Handles timebase combo box changes.
  /// @param index  Selected timebase index
  void onTimebaseChanged(int index);

  /// @brief Handles trigger mode combo box changes.
  /// @param index  Selected trigger mode index (0=Auto, 1=Normal, 2=Single)
  void onTriggerModeChanged(int index);

  /// @brief Updates the cursor readout label with measurement values.
  void updateCursorReadout();

private:
  /// @brief Builds the UI layout with channel list, scope widget, and controls.
  void buildUi();

  OscilloscopeService *service_;         ///< Oscilloscope service for signal data
  QWidget *container_ = nullptr;         ///< Root container widget
  OscilloscopeWidget *scope_ = nullptr;  ///< Custom waveform rendering widget
  QListWidget *channelList_ = nullptr;   ///< Channel list widget
  QComboBox *timebaseCombo_ = nullptr;   ///< Timebase selector combo box
  QComboBox *triggerModeCombo_ = nullptr; ///< Trigger mode selector combo box
  QSpinBox *triggerLevelSpin_ = nullptr; ///< Trigger level adjustment spin box
  QPushButton *runStopBtn_ = nullptr;    ///< Run/Stop acquisition toggle button
  QPushButton *cursorBtn_ = nullptr;     ///< Cursor toggle button
  QLabel *cursorLabel_ = nullptr;        ///< Cursor measurement readout label
  bool cursorActive_ = false;            ///< Whether cursor mode is active
};
