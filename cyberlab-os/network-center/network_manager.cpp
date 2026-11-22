#include "network_manager.h"
#include <QProcess>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

NetworkManagerController::NetworkManagerController(QObject *parent) : QObject(parent) {}

bool NetworkManagerController::runNmcli(const QStringList &args, QString &output) const {
    QProcess process;
    process.start("nmcli", args);
    if (!process.waitForFinished(3000)) {
        return false;
    }
    output = QString::fromUtf8(process.readAllStandardOutput());
    return (process.exitCode() == 0);
}

bool NetworkManagerController::isNetworkManagerAvailable() const {
    QString out;
    return runNmcli({"general", "status"}, out);
}

bool NetworkManagerController::isVirtualBoxDetected() const {
    QFile dmi("/sys/class/dmi/id/product_name");
    if (dmi.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString prod = QTextStream(&dmi).readAll().toLower();
        if (prod.contains("virtualbox") || prod.contains("vbox")) return true;
    }
    QFile hypervisor("/sys/hypervisor/type");
    if (hypervisor.exists()) return true;
    return false;
}

QList<NetworkInterfaceInfo> NetworkManagerController::detectInterfaces() {
    QList<NetworkInterfaceInfo> list;
    QString out;
    if (runNmcli({"device", "status"}, out)) {
        QStringList lines = out.split('\n', Qt::SkipEmptyParts);
        if (lines.size() > 1) {
            for (int i = 1; i < lines.size(); ++i) {
                QStringList parts = lines[i].simplified().split(' ');
                if (parts.size() >= 3) {
                    NetworkInterfaceInfo info;
                    info.name = parts[0];
                    info.type = parts[1];
                    info.state = parts[2];
                    info.isConnected = (info.state == "connected");
                    info.isVirtualBox = isVirtualBoxDetected() && (info.name.startsWith("en") || info.name.startsWith("eth"));
                    list.append(info);
                }
            }
        }
    }
    return list;
}

bool NetworkManagerController::setWifiEnabled(bool enabled) {
    QString out;
    return runNmcli({"radio", "wifi", enabled ? "on" : "off"}, out);
}

bool NetworkManagerController::isWifiEnabled() const {
    QString out;
    if (runNmcli({"radio", "wifi"}, out)) {
        return out.trimmed().toLower() == "enabled";
    }
    return false;
}
