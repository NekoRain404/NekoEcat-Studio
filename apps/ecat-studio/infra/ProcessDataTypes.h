#pragma once

// Shared POD types for PDO maps, Free Run entries, and I/O variables.


#include <QString>

struct PdoMapTableRow {
  // Row model for the PDO mapping table — shows sync manager, PDO, and entry columns.
  int row = -1;
  QString syncManager;
  QString pdo;
  QString index;
  QString subIndex;
  QString bits;
  QString name;
};

struct FreeRunEntryTableRow {
  // Row model for live Free Run entry values. Changed flag tracks value updates
  // between telemetry polls for UI highlighting.
  int row = -1;
  int position = -1;
  bool positionValid = false;
  QString syncManager;
  QString direction;
  QString pdo;
  QString index;
  QString subIndex;
  QString bits;
  QString offset;
  QString bit;
  QString name;
  QString raw;
  QString decoded;
  QString meaning;
  QString mapStatus;
  QString mapDetail;
  bool changed = false;
};

struct IoVariableTableRow {
  // Row model for the I/O variable view — integrates PDO source, PLC mapping,
  // startup SDO config, and live value columns in a single table.
  int row = -1;
  int position = -1;
  bool positionValid = false;
  QString direction;
  QString symbol;
  QString index;
  QString subIndex;
  QString bits;
  QString pdo;
  QString source;
  QString raw;
  QString decoded;
  QString meaning;
  QString watch;
  QString startup;
  QString map;
  QString changed;
  QString plcQuality;
  QString alias;
  QString tags;
  QString note;
};
