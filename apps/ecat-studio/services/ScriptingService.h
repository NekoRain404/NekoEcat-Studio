#pragma once

// ScriptingService — embedded JavaScript scripting engine for automation.
//
// Provides QJSEngine-based script execution with access to all EtherCAT
// services and built-in helper functions for common operations.
//
// When Qt6::Qml is not available, a stub implementation is provided that
// logs warnings and returns empty results.
//
// This service provides JavaScript scripting capabilities for automating
// EtherCAT operations. It handles:
//   - Script execution with QJSEngine
//   - Script file loading and execution
//   - Service registration for script access
//   - Script management (save, load, list)
//   - Built-in helper functions (readSDO, writeSDO, scanTopology, etc.)
//   - Script abortion support
//
// Usage:
//   ServiceContainer *container = ...;
//   ScriptingService *scripting = container->scripting();
//   scripting->registerService("sdo", container->sdo());
//   scripting->registerService("topology", container->topology());
//   QJSValue result = scripting->executeScript("readSDO(0, '0x6000', '0x01')");
//   scripting->executeScriptFile("/path/to/script.js");
//   scripting->saveScript("myScript", "log('Hello')");
//   QString script = scripting->loadScript("myScript");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Script
//   execution is synchronous and blocks the calling thread.
//
// Performance:
//   - Script execution is O(n) where n is script complexity
//   - Service registration is O(1)
//   - Script file I/O is synchronous
//   - QJSEngine provides JIT compilation for repeated scripts

#include <QObject>
#include <QString>
#include <QStringList>

#ifdef ECAT_SCRIPTING_ENABLED
#include <QJSValue>
#endif

class QJSEngine;
class SdoService;
class TopologyService;
class EcatClient;

#ifdef ECAT_SCRIPTING_ENABLED
// Built-in script functions for EtherCAT operations.
class ScriptingBuiltin : public QObject {
  Q_OBJECT
public:
  explicit ScriptingBuiltin(EcatClient *client, SdoService *sdo,
                            TopologyService *topology, QJSEngine *engine,
                            QObject *parent = nullptr);

  // Read an SDO value from a slave.
  // @param position  Slave position
  // @param index     SDO index in hex format
  // @param subIndex  SDO subindex in hex format
  // @return QJSValue with the read result
  Q_INVOKABLE QJSValue readSDO(int position, const QString &index,
                                const QString &subIndex);

  // Write an SDO value to a slave.
  // @param position  Slave position
  // @param index     SDO index in hex format
  // @param subIndex  SDO subindex in hex format
  // @param value     Value to write
  // @param type      Data type
  // @return true if write was successful
  Q_INVOKABLE bool writeSDO(int position, const QString &index,
                             const QString &subIndex, const QString &value,
                             const QString &type);

  // Scan the EtherCAT bus topology.
  // @return QJSValue with scan results
  Q_INVOKABLE QJSValue scanTopology();

  // Set slave state.
  // @param position  Slave position
  // @param state     Target state (OP, PREOP, SAFEOP, INIT)
  // @return true if state change was successful
  Q_INVOKABLE bool setState(int position, const QString &state);

  // Wait for specified milliseconds.
  // @param ms  Milliseconds to wait
  Q_INVOKABLE void wait(int ms);

  // Log a message to the script console.
  // @param message  Message to log
  Q_INVOKABLE void log(const QString &message);

  // Show an alert dialog.
  // @param message  Alert message
  Q_INVOKABLE void alert(const QString &message);

signals:
  // Emitted when a log message is generated.
  // @param message  Log message
  void logMessage(const QString &message);

private:
  EcatClient *client_;       // TCP client to ecatd daemon
  SdoService *sdo_;          // SDO service for read/write
  TopologyService *topology_; // Topology service for scanning
  QJSEngine *engine_;        // JavaScript engine
};
#endif

class ScriptingService : public QObject {
  Q_OBJECT
public:
  explicit ScriptingService(EcatClient *client, SdoService *sdo,
                            TopologyService *topology,
                            QObject *parent = nullptr);

#ifdef ECAT_SCRIPTING_ENABLED
  // Execute a JavaScript script.
  // @param script  JavaScript code to execute
  // @return QJSValue with script result
  QJSValue executeScript(const QString &script);

  // Execute a JavaScript file.
  // @param filePath  Path to the JavaScript file
  // @return QJSValue with script result
  QJSValue executeScriptFile(const QString &filePath);
#else
  // Execute a JavaScript script (stub implementation).
  QVariant executeScript(const QString &script);

  // Execute a JavaScript file (stub implementation).
  QVariant executeScriptFile(const QString &filePath);
#endif

  // Register a service for script access.
  // @param name     Service name for script access
  // @param service  Service object to register
  void registerService(const QString &name, QObject *service);

  // List available scripts.
  // @return QStringList of script names
  QStringList listScripts() const;

  // Save a script to the scripts directory.
  // @param name    Script name
  // @param script  Script content
  // @return true if save was successful
  bool saveScript(const QString &name, const QString &script);

  // Load a script from the scripts directory.
  // @param name  Script name
  // @return Script content
  QString loadScript(const QString &name) const;

  // Abort the currently running script.
  void abortRunning();

signals:
  // Emitted when a script starts execution.
  // @param name  Script name
  void scriptStarted(const QString &name);

#ifdef ECAT_SCRIPTING_ENABLED
  // Emitted when a script completes successfully.
  // @param name    Script name
  // @param result  Script result
  void scriptCompleted(const QString &name, const QJSValue &result);
#else
  // Emitted when a script completes successfully (stub).
  void scriptCompleted(const QString &name, const QVariant &result);
#endif

  // Emitted when a script fails.
  // @param name   Script name
  // @param error  Error message
  void scriptError(const QString &name, const QString &error);

  // Emitted when a log message is generated.
  // @param message  Log message
  void logMessage(const QString &message);

private:
#ifdef ECAT_SCRIPTING_ENABLED
  // Install built-in functions into the engine.
  void installBuiltins();
#endif

  // Get the scripts directory path.
  static QString scriptsDir();
  static bool isValidScriptName(const QString &name);
  static QString scriptPath(const QString &name);

#ifdef ECAT_SCRIPTING_ENABLED
  QJSEngine *engine_ = nullptr;           // JavaScript engine
  ScriptingBuiltin *builtins_ = nullptr;  // Built-in functions
#endif
  bool abortRequested_ = false;           // Abort flag
};
