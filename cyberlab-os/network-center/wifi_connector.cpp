#include "wifi_connector.h"
#include <QProcess>

WifiConnector::WifiConnector(QObject *parent) : QObject(parent) {}

void WifiConnector::connectToNetwork(const QString &ssid, const QString &password, bool isSecured) {
    emit connectionStarted(ssid);

    QProcess *proc = new QProcess(this);
    QStringList args;
    args << "device" << "wifi" << "connect" << ssid;
    if (isSecured && !password.isEmpty()) {
        args << "password" << password;
    }

    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, proc, ssid](int exitCode, QProcess::ExitStatus) {
        QString err = QString::fromUtf8(proc->readAllStandardError());
        QString out = QString::fromUtf8(proc->readAllStandardOutput());
        proc->deleteLater();

        if (exitCode == 0) {
            emit connectionSuccess(ssid);
        } else {
            QString msg = err.isEmpty() ? out : err;
            if (msg.contains("Secrets were required") || msg.contains("auth")) {
                emit connectionFailed(ssid, "Authentication failed: Invalid security key/passphrase.");
            } else {
                emit connectionFailed(ssid, "Failed to connect: " + msg.trimmed());
            }
        }
    });

    proc->start("nmcli", args);
}

void WifiConnector::disconnectFromNetwork(const QString &interfaceName) {
    QProcess::execute("nmcli", {"device", "disconnect", interfaceName});
}
