#include "MockEcatClient.h"

MockEcatClient::MockEcatClient(QObject *parent)
    : EcatClient(parent)
{
}

void MockEcatClient::setConnected(bool connected)
{
    connected_ = connected;
}

void MockEcatClient::setScanResult(const QVector<SlaveInfo> &slaves)
{
    scanResult_ = slaves;
}

void MockEcatClient::setSdoResult(int pos, const QString &idx, const QString &sub, const QString &value)
{
    Q_UNUSED(pos)
    Q_UNUSED(idx)
    Q_UNUSED(sub)
    Q_UNUSED(value)
}

void MockEcatClient::setErrorOnNext(const QString &error)
{
    pendingError_ = error;
}

QVector<MethodCall> MockEcatClient::calls() const
{
    return calls_;
}

int MockEcatClient::callCount(const QString &method) const
{
    int count = 0;
    for (const auto &c : calls_) {
        if (c.method == method)
            count++;
    }
    return count;
}

void MockEcatClient::clearCalls()
{
    calls_.clear();
}

void MockEcatClient::setScanHandler(ScanHandler handler)
{
    scanHandler_ = std::move(handler);
}

void MockEcatClient::triggerSlavesChanged(const QVector<SlaveInfo> &slaves)
{
    emit slavesChanged(slaves);
}

void MockEcatClient::triggerSdoValue(int pos, const QString &idx, const QString &sub, const QString &val)
{
    emit sdoValue(pos, idx, sub, val);
}

void MockEcatClient::triggerError(const QString &msg)
{
    emit errorMessage(msg);
}
