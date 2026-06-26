#pragma once

// EtherCATConfigService — manages local configuration profiles for EtherCAT
// master and slave parameters with validation and JSON persistence.
//
// This service provides configuration management capabilities:
//   - Current profile management with parameter CRUD operations
//   - Named profile save/load/delete with persistence
//   - Profile validation with error and warning reporting
//   - Import/export of local profiles to/from JSON files
//   - Change notifications via Qt signals
//
// This service does not apply parameters to a live EtherCAT master. Runtime
// application must go through an explicit backend-backed operation.
//
// Usage:
//   ServiceContainer *container = ...;
//   EtherCATConfigService *config = container->configService();
//   ConfigProfile profile = config->currentProfile();
//   config->addParameter({"cycleTime", "1000", "us", "DC cycle time", false});
//   ConfigValidation val = config->validateProfile(profile);
//   if (val.valid) config->saveProfile("production");
//   config->loadProfile("production");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.
//
// Performance:
//   - Parameter CRUD is O(n) where n is number of parameters
//   - Profile save/load is O(n) for serialization
//   - Validation is O(n) checking all parameters

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

// Single configuration parameter with metadata.
struct ConfigParameter {
  QString name;           // Parameter name (e.g., "cycleTime")
  QString value;          // Current value as string
  QString unit;           // Unit of measurement (e.g., "us", "bytes")
  QString description;    // Human-readable description
  bool readOnly = false;  // Whether parameter is read-only
};

// Named configuration profile containing a set of parameters.
struct ConfigProfile {
  QString name;                          // Profile name (unique identifier)
  QString description;                   // Human-readable description
  qint64 timestampMs = 0;               // Last modification timestamp
  QVector<ConfigParameter> parameters;   // Parameters in this profile
};

// Result of a profile validation check.
struct ConfigValidation {
  bool valid = false;       // Whether profile passed validation
  QStringList errors;       // Validation errors (blocking)
  QStringList warnings;     // Validation warnings (non-blocking)
};

class EtherCATConfigService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATConfigService(QObject *parent = nullptr);

  // Get the current active configuration profile.
  ConfigProfile currentProfile() const { return profile_; }
  // Get all saved named profiles.
  QVector<ConfigProfile> savedProfiles() const { return savedProfiles_; }

  // Replace the current active profile.
  // Emits configurationChanged() on success.
  void setCurrentProfile(const ConfigProfile &profile);

  // Validate a profile, returning errors and warnings.
  // @param profile  Profile to validate
  // @return Validation result with errors/warnings
  // Emits validationCompleted() with the result.
  ConfigValidation validateProfile(const ConfigProfile &profile) const;

  // Save the current profile under the given name.
  // @param name  Unique profile name
  // @return true if saved successfully
  // Emits profileSaved() on success.
  bool saveProfile(const QString &name);

  // Load a named profile as the current active profile.
  // @param name  Profile name to load
  // @return true if loaded successfully
  // Emits profileLoaded() and configurationChanged() on success.
  bool loadProfile(const QString &name);

  // Delete a saved named profile.
  // @param name  Profile name to delete
  // @return true if deleted, false if not found
  bool deleteProfile(const QString &name);

  // Export a named profile to a file.
  // @param name  Profile name to export
  // @param path  File path to write to
  // @return true if exported successfully
  bool exportProfile(const QString &name, const QString &path) const;

  // Import a profile from an external file.
  // @param path  File path to read from
  // @return true if imported successfully
  bool importProfile(const QString &path);

  // Add a parameter to the current profile.
  // Emits configurationChanged() on success.
  void addParameter(const ConfigParameter &param);

  // Remove a parameter from the current profile by name.
  // Emits configurationChanged() on success.
  void removeParameter(const QString &name);

  // Update a parameter's value in the current profile.
  // Emits configurationChanged() on success.
  void updateParameter(const QString &name, const QString &value);

signals:
  // Emitted when the current profile is modified.
  void configurationChanged(const ConfigProfile &profile);
  // Emitted when a profile is saved to storage.
  void profileSaved(const QString &name);
  // Emitted when a profile is loaded from storage.
  void profileLoaded(const QString &name);
  // Emitted when profile validation completes.
  void validationCompleted(const ConfigValidation &result);

private:
  ConfigProfile profile_;                    // Active configuration profile
  QVector<ConfigProfile> savedProfiles_;     // Persisted named profiles
};
