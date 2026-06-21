#pragma once

// StartupOptimizer — optimizes application startup time.
// Provides lazy initialization, parallel initialization, and caching.
//
// This service provides startup optimization capabilities for the NekoEcat
// Studio application. It handles:
//   - Lazy initialization of services (initialize on first use)
//   - Parallel initialization of plugins
//   - Preloading of frequently used data
//   - Startup metrics tracking and reporting
//   - Service initialization management
//
// Usage:
//   ServiceContainer *container = ...;
//   StartupOptimizer optimizer(container);
//   optimizer.initializeServicesLazy();
//   optimizer.initializePluginsParallel();
//   optimizer.preloadFrequentlyUsedData();
//   StartupMetrics metrics = optimizer.metrics();
//   qint64 totalTime = metrics.totalTimeMs;
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Parallel
//   initialization uses thread-safe mechanisms internally.
//
// Performance:
//   - Lazy initialization defers service creation until first use
//   - Parallel initialization uses multiple threads for plugin setup
//   - Preloading warms caches for frequently accessed data
//   - Metrics tracking adds minimal overhead

#include <QObject>
#include <QElapsedTimer>
#include <QHash>
#include <QMutex>
#include <functional>
#include <memory>

class ServiceContainer;

// Startup performance metrics.
struct StartupMetrics {
  qint64 totalTimeMs = 0;         // Total startup time in milliseconds
  qint64 serviceInitMs = 0;       // Service initialization time
  qint64 pluginInitMs = 0;        // Plugin initialization time
  qint64 uiInitMs = 0;            // UI initialization time
  int servicesInitialized = 0;    // Number of services initialized
  int pluginsInitialized = 0;     // Number of plugins initialized
};

class StartupOptimizer : public QObject {
  Q_OBJECT
public:
  explicit StartupOptimizer(ServiceContainer *container, QObject *parent = nullptr);

  // Initialize services lazily (on first use).
  void initializeServicesLazy();

  // Initialize plugins in parallel using multiple threads.
  void initializePluginsParallel();

  // Preload frequently used data into caches.
  void preloadFrequentlyUsedData();

  // Get the startup performance metrics.
  // @return StartupMetrics structure
  StartupMetrics metrics() const;

signals:
  // Emitted when all initialization is complete.
  void initializationComplete();

  // Emitted when a service is initialized.
  // @param serviceName  Name of the initialized service
  void serviceInitialized(const QString &serviceName);

  // Emitted when a plugin is initialized.
  // @param pluginName  Name of the initialized plugin
  void pluginInitialized(const QString &pluginName);

private:
  // Register a service for lazy initialization.
  void registerLazyService(const QString &name, std::function<void()> initFunc);

  // Initialize a service if it's registered for lazy initialization.
  void initializeServiceIfRegistered(const QString &name);

  ServiceContainer *container_;                      // Service container
  QHash<QString, std::function<void()>> lazyServices_;  // Lazy service initializers
  mutable QMutex mutex_;                             // Thread-safe mutex
  StartupMetrics metrics_;                           // Startup metrics
};
