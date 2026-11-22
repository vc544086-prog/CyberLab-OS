#pragma once
#include <QObject>
#include <QList>
#include <QString>

struct DiscoveredNetwork {
    QString ssid;
    QString bssid;
    int signalStrength; // 0-100%
    QString security;   // WPA2, WPA3, OPEN, etc.
    int channel;
    bool isSecured;
    bool isCurrentlyConnected;
};

class WifiScanner : public QObject {
    Q_OBJECT
public:
    explicit WifiScanner(QObject *parent = nullptr);
    void scanNetworks();
    QList<DiscoveredNetwork> getCachedNetworks() const;

signals:
    void scanCompleted(const QList<DiscoveredNetwork> &networks);
    void scanFailed(const QString &reason);

private:
    QList<DiscoveredNetwork> m_networks;
};
