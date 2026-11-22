#include "network_ui.h"
#include "network_notifications.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSplitter>

NetworkCenterWindow::NetworkCenterWindow(QWidget *parent)
    : QMainWindow(parent),
      m_nm(new NetworkManagerController(this)),
      m_scanner(new WifiScanner(this)),
      m_connector(new WifiConnector(this)),
      m_monitor(new NetworkMonitor(this)) {
    setWindowTitle("CyberLab Network Center v1.0 [Modular D-Bus Manager]");
    resize(820, 580);
    setupCyberTheme();
    buildUI();

    connect(m_scanner, &WifiScanner::scanCompleted, this, &NetworkCenterWindow::onScanFinished);
    connect(m_monitor, &NetworkMonitor::statsUpdated, this, &NetworkCenterWindow::onStatsUpdated);
    connect(m_connector, &WifiConnector::connectionSuccess, this, &NetworkCenterWindow::onConnectionSuccess);
    connect(m_connector, &WifiConnector::connectionFailed, this, &NetworkCenterWindow::onConnectionFailed);

    m_monitor->startMonitoring(1000);
    onRefreshClicked();
}

void NetworkCenterWindow::setupCyberTheme() {
    // Charcoal (#12131C), Electric Purple (#7928CA), Cyan (#00F0FF), White
    setStyleSheet(
        "QMainWindow { background-color: #0d0e15; }"
        "QGroupBox { color: #00f0ff; font-weight: bold; font-size: 13px; border: 1px solid #23273a; border-radius: 8px; margin-top: 12px; padding: 12px; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 6px; }"
        "QLabel { color: #e1e3ea; font-size: 12px; }"
        "QPushButton { background-color: #1e2235; color: #00f0ff; border: 1px solid #3d4465; border-radius: 6px; padding: 7px 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #7928ca; color: #ffffff; border: 1px solid #9945ff; }"
        "QPushButton:pressed { background-color: #55169a; }"
        "QListWidget { background-color: #141622; color: #ffffff; border: 1px solid #23273a; border-radius: 6px; padding: 4px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #1a1e2f; border-radius: 4px; }"
        "QListWidget::item:selected { background-color: #7928ca; color: #00f0ff; }"
        "QLineEdit { background-color: #141622; color: #00f0ff; border: 1px solid #3d4465; border-radius: 6px; padding: 6px; font-size: 13px; }"
        "QProgressBar { background-color: #141622; border: 1px solid #23273a; border-radius: 6px; text-align: center; color: #ffffff; }"
        "QProgressBar::chunk { background-color: #00f0ff; border-radius: 5px; }"
    );
}

void NetworkCenterWindow::buildUI() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // 1. Header Banner
    QGroupBox *statusBox = new QGroupBox("HARDWARE INTERFACES & PLATFORM STATUS", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusBox);
    m_statusLabel = new QLabel("Initializing hardware probing...", this);
    m_hardwareAlertLabel = new QLabel("", this);
    m_hardwareAlertLabel->setStyleSheet("color: #ff9900; font-weight: bold; font-size: 11px;");
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addWidget(m_hardwareAlertLabel);
    mainLayout->addWidget(statusBox);

    // 2. Middle Section: Networks & Connect Panel
    QHBoxLayout *midLayout = new QHBoxLayout();

    // Left: WiFi Network Discovery List
    QGroupBox *listGroup = new QGroupBox("NEARBY WIRELESS NETWORKS", this);
    QVBoxLayout *listVBox = new QVBoxLayout(listGroup);
    m_networkList = new QListWidget(this);
    connect(m_networkList, &QListWidget::itemClicked, this, &NetworkCenterWindow::onNetworkSelected);

    QHBoxLayout *btnBar = new QHBoxLayout();
    m_refreshBtn = new QPushButton("Refresh Discovery", this);
    connect(m_refreshBtn, &QPushButton::clicked, this, &NetworkCenterWindow::onRefreshClicked);
    m_wifiToggleBtn = new QPushButton("Wi-Fi: ON", this);
    connect(m_wifiToggleBtn, &QPushButton::clicked, [this]() {
        bool curr = m_nm->isWifiEnabled();
        m_nm->setWifiEnabled(!curr);
        m_wifiToggleBtn->setText(curr ? "Wi-Fi: OFF" : "Wi-Fi: ON");
        onRefreshClicked();
    });
    btnBar->addWidget(m_refreshBtn);
    btnBar->addWidget(m_wifiToggleBtn);

    listVBox->addWidget(m_networkList);
    listVBox->addLayout(btnBar);
    midLayout->addWidget(listGroup, 3);

    // Right: Connection & Security Dialog
    QGroupBox *connGroup = new QGroupBox("AUTHENTICATION & ACCESS", this);
    QVBoxLayout *connVBox = new QVBoxLayout(connGroup);
    connVBox->addWidget(new QLabel("Security Passphrase (WPA2/WPA3 Personal):", this));
    m_passwordInput = new QLineEdit(this);
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setPlaceholderText("Enter network password...");
    connVBox->addWidget(m_passwordInput);

    m_connectBtn = new QPushButton("Secure Connect", this);
    connect(m_connectBtn, &QPushButton::clicked, this, &NetworkCenterWindow::onConnectClicked);
    connVBox->addWidget(m_connectBtn);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0); // marquee busy
    m_progressBar->setVisible(false);
    connVBox->addWidget(m_progressBar);

    connVBox->addSpacing(16);
    connVBox->addWidget(new QLabel("LIVE TRAFFIC TELEMETRY:", this));
    m_rxTxLabel = new QLabel("RX: 0.0 Kbps | TX: 0.0 Kbps", this);
    m_ipGatewayLabel = new QLabel("IP: Checking... | Gateway: Checking...", this);
    connVBox->addWidget(m_rxTxLabel);
    connVBox->addWidget(m_ipGatewayLabel);
    connVBox->addStretch();

    midLayout->addWidget(connGroup, 2);
    mainLayout->addLayout(midLayout);

    setCentralWidget(central);
}

void NetworkCenterWindow::onRefreshClicked() {
    auto ifaces = m_nm->detectInterfaces();
    QString summary = QString("Detected %1 network interfaces. ").arg(ifaces.size());

    bool hasWifi = false;
    bool inVbox = m_nm->isVirtualBoxDetected();

    for (const auto &iface : ifaces) {
        summary += QString("[%1: %2 - %3] ").arg(iface.name, iface.type, iface.state);
        if (iface.type == "wifi") hasWifi = true;
    }
    m_statusLabel->setText(summary);

    if (inVbox) {
        m_hardwareAlertLabel->setText("VIRTUALBOX DETECTED: Guest OS utilizes Virtual Intel PRO/1000 or PCnet Ethernet NAT/Bridge. Physical host Wi-Fi adapter is encapsulated by hypervisor.");
    } else if (!hasWifi) {
        m_hardwareAlertLabel->setText("NOTICE: No physical 802.11 wireless adapter detected on PCI/USB bus. Ethernet active.");
    } else {
        m_hardwareAlertLabel->setText("PHYSICAL HARDWARE: Realtek/Intel 802.11ac/ax Wi-Fi adapter active and scanning.");
    }

    m_scanner->scanNetworks();
}

void NetworkCenterWindow::onWifiToggled(bool checked) {
    m_nm->setWifiEnabled(checked);
}

void NetworkCenterWindow::onNetworkSelected(QListWidgetItem *item) {
    m_passwordInput->clear();
    m_passwordInput->setFocus();
}

void NetworkCenterWindow::onConnectClicked() {
    QListWidgetItem *item = m_networkList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Select Network", "Please select a wireless network from the list first.");
        return;
    }
    QString ssid = item->text().split(" [").first().trimmed();
    QString pwd = m_passwordInput->text();
    bool isSecured = item->text().contains("WPA") || item->text().contains("SECURE");

    m_progressBar->setVisible(true);
    m_connectBtn->setEnabled(false);
    m_connector->connectToNetwork(ssid, pwd, isSecured);
}

void NetworkCenterWindow::onScanFinished(const QList<DiscoveredNetwork> &networks) {
    m_networkList->clear();
    for (const auto &n : networks) {
        QString icon = n.isSecured ? "[LOCKED]" : "[OPEN]";
        QString connStr = n.isCurrentlyConnected ? " (CONNECTED)" : "";
        QString text = QString("%1 [%2% - %3 %4]%5")
                       .arg(n.ssid)
                       .arg(n.signalStrength)
                       .arg(n.security)
                       .arg(icon)
                       .arg(connStr);
        QListWidgetItem *item = new QListWidgetItem(text, m_networkList);
        if (n.isCurrentlyConnected) {
            item->setForeground(QColor("#00f0ff"));
        }
    }
}

void NetworkCenterWindow::onStatsUpdated(const TrafficStats &stats) {
    m_rxTxLabel->setText(QString("DL: %1 Kbps | UL: %2 Kbps | Uptime: %3s")
                         .arg(stats.rxRateKbps, 0, 'f', 1)
                         .arg(stats.txRateKbps, 0, 'f', 1)
                         .arg(stats.connectionUptimeSeconds));
    m_ipGatewayLabel->setText(QString("Gateway: %1 | Transferred: %2 MB")
                              .arg(stats.gateway.isEmpty() ? "N/A" : stats.gateway)
                              .arg((stats.rxBytes + stats.txBytes) / (1024 * 1024)));
}

void NetworkCenterWindow::onConnectionSuccess(const QString &ssid) {
    m_progressBar->setVisible(false);
    m_connectBtn->setEnabled(true);
    NetworkNotifications::notify("CyberLab Network Center", "Successfully connected to " + ssid);
    onRefreshClicked();
}

void NetworkCenterWindow::onConnectionFailed(const QString &ssid, const QString &errorMsg) {
    m_progressBar->setVisible(false);
    m_connectBtn->setEnabled(true);
    QMessageBox::critical(this, "Connection Error", errorMsg);
    NetworkNotifications::notify("Network Error", errorMsg, true);
}
