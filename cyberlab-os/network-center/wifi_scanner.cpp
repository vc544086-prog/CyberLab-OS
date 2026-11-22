#include "wifi_scanner.h"
#include <QProcess>
#include <QStringList>

WifiScanner::WifiScanner(QObject *parent) : QObject(parent) {}

QList<DiscoveredNetwork> WifiScanner::getCachedNetworks() const {
    return m_networks;
}

void WifiScanner::scanNetworks() {
    QProcess process;
    // Request fresh hardware scan then list
    process.start("nmcli", {"device", "wifi", "rescan"});
    process.waitForFinished(2000);

    QProcess listProc;
    listProc.start("nmcli", {"-t", "-f", "IN-USE,SSID,BSSID,SIGNAL,SECURITY,CHAN", "device", "wifi", "list"});
    if (!listProc.waitForFinished(3000)) {
        emit scanFailed("Timeout scanning Wi-Fi hardware interfaces.");
        return;
    }

    m_networks.clear();
    QString out = QString::fromUtf8(listProc.readAllStandardOutput());
    QStringList lines = out.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(':');
        if (parts.size() >= 5) {
            DiscoveredNetwork net;
            net.isCurrentlyConnected = (parts[0] == "*");
            net.ssid = parts[1].isEmpty() ? "[Hidden SSID]" : parts[1];
            net.bssid = parts[2];
            net.signalStrength = parts[3].toInt();
            net.security = parts[4].isEmpty() ? "OPEN" : parts[4];
            net.isSecured = (net.security != "OPEN" && !net.security.isEmpty());
            net.channel = (parts.size() > 5) ? parts[5].toInt() : 1;
            m_networks.append(net);
        }
    }
    emit scanCompleted(m_networks);
}
