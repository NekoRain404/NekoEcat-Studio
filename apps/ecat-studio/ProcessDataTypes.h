#pragma once

#include <QString>

struct PdoMapTableRow {
  int row = -1;
  QString syncManager;
  QString pdo;
  QString index;
  QString subIndex;
  QString bits;
  QString name;
};

struct FreeRunEntryTableRow {
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
