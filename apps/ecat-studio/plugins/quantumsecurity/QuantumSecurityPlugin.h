#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QPushButton;
class QTabWidget;
class QTableWidget;

class QuantumSecurityPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit QuantumSecurityPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct QuantumKey {
    QString id;
    QString algorithm;
    int keySize;
    QDateTime createdAt;
    QDateTime expiresAt;
    bool active;
  };

  struct QuantumRandom {
    QString id;
    int bitLength;
    QString entropy;
    QDateTime generatedAt;
    QString source;
  };

  struct QuantumEncryption {
    QString id;
    QString algorithm;
    QString inputHash;
    QString outputHash;
    QDateTime timestamp;
    bool success;
  };

  struct QuantumSignature {
    QString id;
    QString signer;
    QString messageHash;
    QString signature;
    QDateTime timestamp;
    bool verified;
  };

  void addKey(const QuantumKey &key);
  void removeKey(int index);
  int keyCount() const;

  void addRandom(const QuantumRandom &random);
  void removeRandom(int index);
  int randomCount() const;

  void addEncryption(const QuantumEncryption &encryption);
  void removeEncryption(int index);
  int encryptionCount() const;

  void addSignature(const QuantumSignature &signature);
  void removeSignature(int index);
  int signatureCount() const;

  void verifySignature(int index);
  QString exportReport() const;

  QTabWidget *tabs() const;
  QTableWidget *keyTable() const;
  QTableWidget *randomTable() const;
  QTableWidget *encryptionTable() const;
  QTableWidget *signatureTable() const;
  QLabel *statusLabel() const;

signals:
  void keyGenerated(const QString &keyId);
  void randomGenerated(const QString &randomId);
  void encryptionCompleted(const QString &encryptionId, bool success);
  void signatureVerified(const QString &signatureId, bool valid);

private:
  void buildUi();
  void rebuildKeyTable();
  void rebuildRandomTable();
  void rebuildEncryptionTable();
  void rebuildSignatureTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QTableWidget *keyTable_ = nullptr;
  QTableWidget *randomTable_ = nullptr;
  QTableWidget *encryptionTable_ = nullptr;
  QTableWidget *signatureTable_ = nullptr;
  QPushButton *addKeyBtn_ = nullptr;
  QPushButton *removeKeyBtn_ = nullptr;
  QPushButton *generateRandomBtn_ = nullptr;
  QPushButton *removeRandomBtn_ = nullptr;
  QPushButton *addEncryptionBtn_ = nullptr;
  QPushButton *removeEncryptionBtn_ = nullptr;
  QPushButton *addSignatureBtn_ = nullptr;
  QPushButton *removeSignatureBtn_ = nullptr;
  QPushButton *verifySignatureBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<QuantumKey> keys_;
  QVector<QuantumRandom> randoms_;
  QVector<QuantumEncryption> encryptions_;
  QVector<QuantumSignature> signatures_;
};
