#include "network_monitor.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>

NetworkMonitor::NetworkMonitor(QObject *parent)
    : QObject(parent), m_timer(new QTimer(this)), m_prevRx(0), m_prevTx(0), m_uptime(0) {
    connect(m_timer, &QTimer::timeout, this, &NetworkMonitor::sampleNetwork);
}

void NetworkMonitor::startMonitoring(int intervalMs) {
    m_timer->start(intervalMs);
}

void NetworkMonitor::stopMonitoring() {
    m_timer->stop();
}

TrafficStats NetworkMonitor::getStats() const {
    return m_currentStats;
}

void NetworkMonitor::sampleNetwork() {
    QFile devFile("/proc/net/dev");
    if (devFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&devFile);
        quint64 totalRx = 0, totalTx = 0;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.contains(':') && !line.startsWith("lo:")) {
                QStringList parts = line.split(QRegularExpression("[:\\s]+"), Qt::SkipEmptyParts);
                if (parts.size() >= 10) {
                    totalRx += parts[1].toULongLong();
                    totalTx += parts[9].toULongLong();
                }
            }
        }

        if (m_prevRx > 0) {
            m_currentStats.rxRateKbps = ((totalRx - m_prevRx) * 8.0) / 1024.0;
            m_currentStats.txRateKbps = ((totalTx - m_prevTx) * 8.0) / 1024.0;
        }
        m_prevRx = totalRx;
        m_prevTx = totalTx;
        m_currentStats.rxBytes = totalRx;
        m_currentStats.txBytes = totalTx;
    }

    m_uptime += 1;
    m_currentStats.connectionUptimeSeconds = m_uptime;

    // Read IP & Gateway
    QProcess ipProc;
    ipProc.start("ip", {"route", "show", "default"});
    if (ipProc.waitForFinished(500)) {
        QString line = QString::fromUtf8(ipProc.readAllStandardOutput());
        QStringList parts = line.simplified().split(' ');
        if (parts.size() >= 3 && parts[1] == "via") {
            m_currentStats.gateway = parts[2];
        }
    }

    emit statsUpdated(m_currentStats);
}
