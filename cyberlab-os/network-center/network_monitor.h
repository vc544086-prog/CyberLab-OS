#pragma once
#include <QObject>
#include <QTimer>
#include <QString>

struct TrafficStats {
    quint64 rxBytes;
    quint64 txBytes;
    double rxRateKbps;
    double txRateKbps;
    QString ipAddress;
    QString gateway;
    QString dns;
    quint64 connectionUptimeSeconds;
};

class NetworkMonitor : public QObject {
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = nullptr);
    void startMonitoring(int intervalMs = 1000);
    void stopMonitoring();
    TrafficStats getStats() const;

signals:
    void statsUpdated(const TrafficStats &stats);

private slots:
    void sampleNetwork();

private:
    QTimer *m_timer;
    quint64 m_prevRx;
    quint64 m_prevTx;
    quint64 m_uptime;
    TrafficStats m_currentStats;
};
