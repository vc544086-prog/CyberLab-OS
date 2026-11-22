#pragma once
#include <QObject>
#include <QString>

class NetworkNotifications : public QObject {
    Q_OBJECT
public:
    explicit NetworkNotifications(QObject *parent = nullptr);
    static void notify(const QString &title, const QString &message, bool isError = false);
};
