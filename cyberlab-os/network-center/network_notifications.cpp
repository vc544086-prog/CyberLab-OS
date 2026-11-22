#include "network_notifications.h"
#include <QProcess>

NetworkNotifications::NetworkNotifications(QObject *parent) : QObject(parent) {}

void NetworkNotifications::notify(const QString &title, const QString &message, bool isError) {
    QString icon = isError ? "dialog-error" : "network-wireless";
    QProcess::startDetached("notify-send", {"-i", icon, title, message});
}
