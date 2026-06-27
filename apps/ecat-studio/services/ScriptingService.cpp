#include "ScriptingService.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

// ScriptingService.cpp — JavaScript scripting engine with EtherCAT builtin functions
//
// Implementation notes:
//   - Conditionally compiled behind ECAT_SCRIPTING_ENABLED (requires Qt6::Qml)
//   - Exposes readSDO, writeSDO, scanTopology, setState, wait, log, alert as globals
//   - Scripts stored in AppDataLocation/scripts/; supports inline and file execution

#ifdef ECAT_SCRIPTING_ENABLED
#include "infra/EcatClient.h"
#include "SdoService.h"
#include "TopologyService.h"
#include <QJSEngine>
#include <QThread>

// ── ScriptingBuiltin ──────────────────────────────────────────────────────

ScriptingBuiltin::ScriptingBuiltin(EcatClient *client, SdoService *sdo,
                                   TopologyService *topology, QJSEngine *engine,
                                   QObject *parent)
    : QObject(parent), client_(client), sdo_(sdo), topology_(topology),
      engine_(engine) {}

QJSValue ScriptingBuiltin::readSDO(int position, const QString &index,
                                    const QString &subIndex) {
  if (!client_->isConnected()) return QJSValue(QJSValue::NullValue);
  sdo_->upload(position, index, subIndex);
  return QJSValue(QStringLiteral("pending"));
}

bool ScriptingBuiltin::writeSDO(int position, const QString &index,
                                 const QString &subIndex, const QString &value,
                                 const QString &type) {
  if (!client_->isConnected()) return false;
  sdo_->download(position, index, subIndex, value, type);
  emit logMessage(QStringLiteral("writeSDO request queued; wait for SDO/daemon confirmation"));
  return false;
}

QJSValue ScriptingBuiltin::scanTopology() {
  if (!client_->isConnected()) return engine_->newArray(0);
  topology_->scan();
  QJSValue arr = engine_->newArray(0);
  const auto slaves = topology_->currentSlaves();
  for (int i = 0; i < slaves.size(); ++i) {
    QJSValue obj = engine_->newObject();
    obj.setProperty(QStringLiteral("position"), slaves[i].position);
    obj.setProperty(QStringLiteral("name"), slaves[i].name);
    obj.setProperty(QStringLiteral("state"), slaves[i].state);
    arr.setProperty(i, obj);
  }
  return arr;
}

bool ScriptingBuiltin::setState(int position, const QString &state) {
  if (!client_->isConnected()) return false;
  client_->setState(position, state);
  emit logMessage(QStringLiteral("setState request queued; wait for daemon confirmation"));
  return false;
}

void ScriptingBuiltin::wait(int ms) {
  QThread::msleep(static_cast<unsigned long>(ms));
}

void ScriptingBuiltin::log(const QString &message) {
  emit logMessage(message);
}

void ScriptingBuiltin::alert(const QString &message) {
  emit logMessage(QStringLiteral("[ALERT] ") + message);
}
#endif // ECAT_SCRIPTING_ENABLED

// ── ScriptingService ──────────────────────────────────────────────────────

ScriptingService::ScriptingService(EcatClient *client, SdoService *sdo,
                                   TopologyService *topology,
                                   QObject *parent)
    : QObject(parent) {
#ifdef ECAT_SCRIPTING_ENABLED
  engine_ = new QJSEngine(this);
  builtins_ = new ScriptingBuiltin(client, sdo, topology, engine_, this);
  connect(builtins_, &ScriptingBuiltin::logMessage,
          this, &ScriptingService::logMessage);
  installBuiltins();
#else
  Q_UNUSED(client);
  Q_UNUSED(sdo);
  Q_UNUSED(topology);
  qWarning() << "ScriptingService: Qt6::Qml not available, scripting is disabled";
#endif
}

#ifdef ECAT_SCRIPTING_ENABLED
void ScriptingService::installBuiltins() {
  QJSValue builtinObj = engine_->newQObject(builtins_);
  engine_->globalObject().setProperty(QStringLiteral("EtherCAT"), builtinObj);

  QJSValue global = engine_->globalObject();
  global.setProperty(QStringLiteral("readSDO"),
                     builtinObj.property(QStringLiteral("readSDO")));
  global.setProperty(QStringLiteral("writeSDO"),
                     builtinObj.property(QStringLiteral("writeSDO")));
  global.setProperty(QStringLiteral("scanTopology"),
                     builtinObj.property(QStringLiteral("scanTopology")));
  global.setProperty(QStringLiteral("setState"),
                     builtinObj.property(QStringLiteral("setState")));
  global.setProperty(QStringLiteral("wait"),
                     builtinObj.property(QStringLiteral("wait")));
  global.setProperty(QStringLiteral("log"),
                     builtinObj.property(QStringLiteral("log")));
  global.setProperty(QStringLiteral("alert"),
                     builtinObj.property(QStringLiteral("alert")));
}
#endif

#ifdef ECAT_SCRIPTING_ENABLED
QJSValue ScriptingService::executeScript(const QString &script) {
  abortRequested_ = false;
  emit scriptStarted(QStringLiteral("<inline>"));

  QJSValue result = engine_->evaluate(script);
  if (result.isError()) {
    QString errMsg = result.toString() + " (line " +
                     result.property(QStringLiteral("lineNumber")).toString() +
                     ")";
    emit scriptError(QStringLiteral("<inline>"), errMsg);
    return result;
  }

  emit scriptCompleted(QStringLiteral("<inline>"), result);
  return result;
}

QJSValue ScriptingService::executeScriptFile(const QString &filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit scriptError(filePath, QStringLiteral("Cannot open file: ") + filePath);
    return QJSValue(QJSValue::UndefinedValue);
  }
  QString script = QString::fromUtf8(file.readAll());
  QString name = QFileInfo(filePath).baseName();

  abortRequested_ = false;
  emit scriptStarted(name);

  QJSValue result = engine_->evaluate(script, filePath);
  if (result.isError()) {
    QString errMsg =
        result.toString() + " (line " +
        result.property(QStringLiteral("lineNumber")).toString() + ")";
    emit scriptError(name, errMsg);
    return result;
  }

  emit scriptCompleted(name, result);
  return result;
}
#else
QVariant ScriptingService::executeScript(const QString &script) {
  Q_UNUSED(script);
  emit scriptError(QStringLiteral("<inline>"),
                   QStringLiteral("Scripting not available: Qt6::Qml not installed"));
  return QVariant();
}

QVariant ScriptingService::executeScriptFile(const QString &filePath) {
  Q_UNUSED(filePath);
  emit scriptError(filePath,
                   QStringLiteral("Scripting not available: Qt6::Qml not installed"));
  return QVariant();
}
#endif

void ScriptingService::registerService(const QString &name, QObject *service) {
#ifdef ECAT_SCRIPTING_ENABLED
  QJSValue obj = engine_->newQObject(service);
  engine_->globalObject().setProperty(name, obj);
#else
  Q_UNUSED(name);
  Q_UNUSED(service);
#endif
}

QStringList ScriptingService::listScripts() const {
  QDir dir(scriptsDir());
  if (!dir.exists()) return {};
  return dir.entryList({QStringLiteral("*.js")}, QDir::Files, QDir::Name);
}

bool ScriptingService::saveScript(const QString &name, const QString &script) {
  if (!isValidScriptName(name)) return false;

  QDir dir;
  if (!dir.mkpath(scriptsDir())) return false;
  const QString path = scriptPath(name);
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
  const QByteArray bytes = script.toUtf8();
  return file.write(bytes) == bytes.size() && file.flush();
}

QString ScriptingService::loadScript(const QString &name) const {
  if (!isValidScriptName(name)) return {};

  const QString path = scriptPath(name);
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
  return QString::fromUtf8(file.readAll());
}

void ScriptingService::abortRunning() { abortRequested_ = true; }

QString ScriptingService::scriptsDir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
         QDir::separator() + QStringLiteral("scripts");
}

bool ScriptingService::isValidScriptName(const QString &name) {
  if (name.isEmpty()) return false;
  if (name.size() > 128) return false;

  const QFileInfo info(name);
  if (info.isAbsolute()) return false;
  if (name == QStringLiteral(".") || name == QStringLiteral("..")) return false;
  if (name.contains(QLatin1Char('/')) || name.contains(QLatin1Char('\\'))) return false;
  return true;
}

QString ScriptingService::scriptPath(const QString &name) {
  return QDir(scriptsDir()).filePath(name + QStringLiteral(".js"));
}
