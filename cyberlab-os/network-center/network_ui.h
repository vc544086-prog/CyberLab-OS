#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QLineEdit>
#include "network_manager.h"
#include "wifi_scanner.h"
#include "wifi_connector.h"
#include "network_monitor.h"

class NetworkCenterWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit NetworkCenterWindow(QWidget *parent = nullptr);

private slots:
    void onRefreshClicked();
    void onWifiToggled(bool checked);
    void onNetworkSelected(QListWidgetItem *item);
    void onConnectClicked();
    void onScanFinished(const QList<DiscoveredNetwork> &networks);
    void onStatsUpdated(const TrafficStats &stats);
    void onConnectionSuccess(const QString &ssid);
    void onConnectionFailed(const QString &ssid, const QString &errorMsg);

private:
    void setupCyberTheme();
    void buildUI();

    NetworkManagerController *m_nm;
    WifiScanner *m_scanner;
    WifiConnector *m_connector;
    NetworkMonitor *m_monitor;

    QLabel *m_statusLabel;
    QLabel *m_hardwareAlertLabel;
    QLabel *m_rxTxLabel;
    QLabel *m_ipGatewayLabel;
    QPushButton *m_wifiToggleBtn;
    QPushButton *m_refreshBtn;
    QListWidget *m_networkList;
    QLineEdit *m_passwordInput;
    QPushButton *m_connectBtn;
    QProgressBar *m_progressBar;
};
