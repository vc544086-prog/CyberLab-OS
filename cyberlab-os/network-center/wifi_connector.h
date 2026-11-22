#pragma once
#include <QObject>
#include <QString>

class WifiConnector : public QObject {
    Q_OBJECT
public:
    explicit WifiConnector(QObject *parent = nullptr);
    void connectToNetwork(const QString &ssid, const QString &password, bool isSecured);
    void disconnectFromNetwork(const QString &interfaceName);

signals:
    void connectionStarted(const QString &ssid);
    void connectionSuccess(const QString &ssid);
    void connectionFailed(const QString &ssid, const QString &errorMsg);
};
