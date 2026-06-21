#pragma once

// ResourceManagementService -- manages resources for EtherCAT projects.
//
// Supports allocating, tracking, optimizing, and reporting on resources
// with availability, cost, and skill tracking.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class ResourceType { Human, Hardware, Software, Equipment, Facility };
enum class ResourceStatus { Available, Allocated, Maintenance, Unavailable };

struct ResourceConfig {
  QString name;
  ResourceType type = ResourceType::Human;
  int capacity = 100;
  bool available = true;
  double costPerHour = 0.0;
  QString location;
  QStringList skills;
};

struct Resource {
  int id = 0;
  QString name;
  ResourceType type = ResourceType::Human;
  int capacity = 100;
  int currentLoad = 0;
  bool available = true;
  double costPerHour = 0.0;
  QString location;
  QStringList skills;
  ResourceStatus status = ResourceStatus::Available;
  QDateTime createdAt;
  QDateTime updatedAt;
};

struct ResourceStatusInfo {
  int resourceId = 0;
  ResourceStatus status = ResourceStatus::Available;
  int currentLoad = 0;
  int capacity = 100;
  int utilizationPercent = 0;
  QDateTime lastUpdated;
};

struct ResourceAllocationEntry {
  int resourceId = 0;
  int projectId = 0;
  int allocationPercent = 0;
  QDateTime startDate;
  QDateTime endDate;
  bool active = true;
};

struct ResourceOptimizationSuggestion {
  int resourceId = 0;
  QString resourceName;
  QString suggestion;
  int potentialSaving = 0;
};

struct ResourceOptimizationResult {
  QVector<ResourceOptimizationSuggestion> suggestions;
  double totalPotentialSaving = 0.0;
  QDateTime generatedAt;
};

struct ResourceReport {
  int totalResources = 0;
  int availableResources = 0;
  int allocatedResources = 0;
  int maintenanceResources = 0;
  double totalCostPerHour = 0.0;
  QVector<Resource> resources;
  QVector<ResourceAllocationEntry> allocations;
  QDateTime generatedAt;
};

class ResourceManagementService : public QObject {
  Q_OBJECT
public:
  explicit ResourceManagementService(QObject *parent = nullptr);

  Resource allocateResource(const ResourceConfig &config);
  ResourceStatusInfo trackResource(int resourceId) const;
  ResourceOptimizationResult optimizeResources() const;
  ResourceReport generateResourceReport() const;

  bool updateResourceStatus(int resourceId, ResourceStatus status);
  bool allocateToProject(int resourceId, int projectId, int percent);
  bool releaseFromProject(int resourceId, int projectId);
  bool updateLoad(int resourceId, int load);

  Resource resource(int resourceId) const;
  QVector<Resource> allResources() const;
  QVector<Resource> resourcesByType(ResourceType type) const;
  QVector<Resource> availableResources() const;
  QVector<ResourceAllocationEntry> resourceAllocations(int resourceId) const;
  int resourceCount() const;

signals:
  void resourceAllocated(const Resource &resource);
  void resourceUpdated(const ResourceStatusInfo &status);

private:
  QHash<int, Resource> resources_;
  QHash<int, QVector<ResourceAllocationEntry>> allocations_;
  int nextResourceId_ = 1;
  static constexpr int kMaxResources = 5000;
};
