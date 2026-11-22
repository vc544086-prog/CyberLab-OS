#pragma once
#include <QObject>
#include <QString>
#include <QList>

struct NetworkInterfaceInfo {
    QString name;
    QString type; // "wifi", "ethernet", "virtual", "loopback"
    QString macAddress;
    QString ipAddress;
    QString state;
    bool isConnected;
    bool isVirtualBox;
};

class NetworkManagerController : public QObject {
    Q_OBJECT
public:
    explicit NetworkManagerController(QObject *parent = nullptr);
    bool isNetworkManagerAvailable() const;
    QList<NetworkInterfaceInfo> detectInterfaces();
    bool setWifiEnabled(bool enabled);
    bool isWifiEnabled() const;
    bool isVirtualBoxDetected() const;

signals:
    void interfaceStateChanged();
    void errorOccurred(const QString &error);

private:
    bool runNmcli(const QStringList &args, QString &output) const;
};
