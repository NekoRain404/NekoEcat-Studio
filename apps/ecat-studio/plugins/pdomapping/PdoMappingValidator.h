#pragma once

#include <QString>
#include <QVector>

struct SyncManagerBlock;
struct PdoCanvasEntry;

struct PdoValidationError {
  enum class Type { SizeExceeded, DuplicateEntry, OverlappingAddress, MissingRequired, InvalidBitSize };
  Type type;
  QString message;
  int smIndex = -1;
  int entryIndex = -1;
};

struct PdoValidationReport {
  bool valid = true;
  QVector<PdoValidationError> errors;
  int totalInputBits = 0;
  int totalOutputBits = 0;
  int maxBitsPerSm = 1500 * 8;
};

class PdoMappingValidator {
public:
  static PdoValidationReport validate(const QVector<SyncManagerBlock> &sms);
  static PdoValidationReport validateSmSize(const QVector<SyncManagerBlock> &sms);
  static PdoValidationReport validateDuplicates(const QVector<SyncManagerBlock> &sms);
  static PdoValidationReport validateAddresses(const QVector<SyncManagerBlock> &sms);
  static PdoValidationReport validateRequired(const QVector<SyncManagerBlock> &sms);
  static QString errorString(const PdoValidationError &error);
};
