#include "wifi_scanner.h"
#include <QProcess>
#include <QStringList>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

WifiScanner::WifiScanner(QObject *parent) : QObject(parent) {}

QList<DiscoveredNetwork> WifiScanner::getCachedNetworks() const {
    return m_networks;
}

void WifiScanner::scanNetworks() {
    m_networks.clear();

    // 1. Try native physical wireless hardware scan
    QProcess listProc;
    listProc.start("nmcli", {"-t", "-f", "IN-USE,SSID,BSSID,SIGNAL,SECURITY,CHAN", "device", "wifi", "list"});
    if (listProc.waitForFinished(2000)) {
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
    }

    // 2. Hypervisor Passthrough / Bridged Host Telemetry
    // VirtualBox guests access host wireless scan data via VBoxControl guest properties
    QProcess vbProc;
    vbProc.start("VBoxControl", {"-nologo", "guestproperty", "get", "/VirtualBox/HostInfo/Wlan/AvailableNetworks"});
    if (vbProc.waitForFinished(1000) && vbProc.exitCode() == 0) {
        QString raw = QString::fromUtf8(vbProc.readAllStandardOutput());
        // Value: Value: SSID (Signal); ...
        if (raw.contains("Value:")) {
            QString netStr = raw.section("Value:", 1).trimmed();
            QStringList netItems = netStr.split(';', Qt::SkipEmptyParts);
            
            // Get active connected SSID
            QProcess activeProc;
            activeProc.start("VBoxControl", {"-nologo", "guestproperty", "get", "/VirtualBox/HostInfo/Wlan/ActiveSSID"});
            QString activeSSID = "";
            if (activeProc.waitForFinished(500)) {
                activeSSID = QString::fromUtf8(activeProc.readAllStandardOutput()).section("Value:", 1).trimmed();
            }

            for (const QString &item : netItems) {
                QString entry = item.trimmed();
                QRegularExpression re(R"((.+)\s*\((\d+)%\))");
                QRegularExpressionMatch m = re.match(entry);
                if (m.hasMatch()) {
                    QString ssid = m.captured(1).trimmed();
                    int sig = m.captured(2).toInt();

                    DiscoveredNetwork d;
                    d.ssid = ssid;
                    d.signalStrength = sig;
                    d.security = "WPA2-Personal (AES)";
                    d.isSecured = true;
                    d.channel = 44;
                    d.isCurrentlyConnected = (activeSSID.length() > 0 && ssid == activeSSID);
                    d.bssid = d.isCurrentlyConnected ? "Bridged NIC (Host)" : "Host-Telemetry";

                    // Avoid duplicate entries
                    bool exists = false;
                    for (const auto &ex : m_networks) {
                        if (ex.ssid == d.ssid) { exists = true; break; }
                    }
                    if (!exists) {
                        m_networks.append(d);
                    }
                }
            }
        }
    }

    emit scanCompleted(m_networks);
}
