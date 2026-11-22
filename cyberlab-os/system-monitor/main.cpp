#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QTextStream>

class SystemMonitorWindow : public QMainWindow {
public:
    SystemMonitorWindow() {
        setWindowTitle("CyberLab System Monitor [Real-Time /proc Telemetry]");
        resize(860, 600);
        setupTheme();
        buildUI();

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SystemMonitorWindow::refreshMetrics);
        timer->start(1000);
        refreshMetrics();
    }

private:
    void setupTheme() {
        setStyleSheet(
            "QMainWindow { background-color: #0d0e15; }"
            "QGroupBox { color: #00f0ff; font-weight: bold; font-size: 13px; border: 1px solid #23273a; border-radius: 8px; margin-top: 10px; padding: 10px; }"
            "QLabel { color: #e1e3ea; font-size: 12px; }"
            "QProgressBar { background-color: #141622; border: 1px solid #23273a; border-radius: 6px; text-align: center; color: #ffffff; }"
            "QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7928ca, stop:1 #00f0ff); border-radius: 5px; }"
            "QTableWidget { background-color: #141622; color: #ffffff; border: 1px solid #23273a; gridline-color: #1a1e2f; }"
            "QHeaderView::section { background-color: #1a1e2f; color: #00f0ff; padding: 4px; border: 1px solid #23273a; }"
        );
    }

    void buildUI() {
        QWidget *central = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        // Hardware Gauges
        QGroupBox *gaugeBox = new QGroupBox("HARDWARE RESOURCE CONSUMPTION", this);
        QGridLayout *gLayout = new QGridLayout(gaugeBox);

        gLayout->addWidget(new QLabel("CPU Utilization:", this), 0, 0);
        m_cpuBar = new QProgressBar(this);
        m_cpuBar->setRange(0, 100);
        gLayout->addWidget(m_cpuBar, 0, 1);
        m_cpuLabel = new QLabel("0%", this);
        gLayout->addWidget(m_cpuLabel, 0, 2);

        gLayout->addWidget(new QLabel("Physical RAM:", this), 1, 0);
        m_ramBar = new QProgressBar(this);
        m_ramBar->setRange(0, 100);
        gLayout->addWidget(m_ramBar, 1, 1);
        m_ramLabel = new QLabel("0 / 0 MB", this);
        gLayout->addWidget(m_ramLabel, 1, 2);

        gLayout->addWidget(new QLabel("Swap Space:", this), 2, 0);
        m_swapBar = new QProgressBar(this);
        m_swapBar->setRange(0, 100);
        gLayout->addWidget(m_swapBar, 2, 1);
        m_swapLabel = new QLabel("0 / 0 MB", this);
        gLayout->addWidget(m_swapLabel, 2, 2);

        mainLayout->addWidget(gaugeBox);

        // System Info Label
        m_infoLabel = new QLabel("Uptime: Reading... | Linux Kernel: Reading...", this);
        m_infoLabel->setStyleSheet("color: #7928ca; font-weight: bold; margin: 4px;");
        mainLayout->addWidget(m_infoLabel);

        // Process Table
        QGroupBox *procBox = new QGroupBox("ACTIVE PROCESS TABLE (/proc)", this);
        QVBoxLayout *pLayout = new QVBoxLayout(procBox);
        m_procTable = new QTableWidget(this);
        m_procTable->setColumnCount(4);
        m_procTable->setHorizontalHeaderLabels({"PID", "Process Name", "State", "Memory (RSS)"});
        m_procTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        pLayout->addWidget(m_procTable);
        mainLayout->addWidget(procBox);

        setCentralWidget(central);
    }

    void refreshMetrics() {
        // Read /proc/meminfo
        QFile memFile("/proc/meminfo");
        quint64 memTotal = 0, memFree = 0, memAvail = 0, swapTotal = 0, swapFree = 0;
        if (memFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&memFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.startsWith("MemTotal:")) memTotal = line.split(QRegularExpression("\\s+"))[1].toULongLong();
                else if (line.startsWith("MemFree:")) memFree = line.split(QRegularExpression("\\s+"))[1].toULongLong();
                else if (line.startsWith("MemAvailable:")) memAvail = line.split(QRegularExpression("\\s+"))[1].toULongLong();
                else if (line.startsWith("SwapTotal:")) swapTotal = line.split(QRegularExpression("\\s+"))[1].toULongLong();
                else if (line.startsWith("SwapFree:")) swapFree = line.split(QRegularExpression("\\s+"))[1].toULongLong();
            }
        }

        if (memTotal > 0) {
            quint64 usedMem = memTotal - memAvail;
            int memPct = (usedMem * 100) / memTotal;
            m_ramBar->setValue(memPct);
            m_ramLabel->setText(QString("%1 / %2 MB (%3%)").arg(usedMem / 1024).arg(memTotal / 1024).arg(memPct));
        }

        if (swapTotal > 0) {
            quint64 usedSwap = swapTotal - swapFree;
            int swapPct = (usedSwap * 100) / swapTotal;
            m_swapBar->setValue(swapPct);
            m_swapLabel->setText(QString("%1 / %2 MB").arg(usedSwap / 1024).arg(swapTotal / 1024));
        } else {
            m_swapBar->setValue(0);
            m_swapLabel->setText("No Swap Configured");
        }

        // Read CPU stat
        static quint64 lastIdle = 0, lastTotal = 0;
        QFile statFile("/proc/stat");
        if (statFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString line = QTextStream(&statFile).readLine();
            QStringList p = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (p.size() >= 5) {
                quint64 user = p[1].toULongLong();
                quint64 nice = p[2].toULongLong();
                quint64 sys  = p[3].toULongLong();
                quint64 idle = p[4].toULongLong();
                quint64 total = user + nice + sys + idle;

                if (lastTotal > 0 && total > lastTotal) {
                    quint64 dTotal = total - lastTotal;
                    quint64 dIdle = idle - lastIdle;
                    int cpuPct = ((dTotal - dIdle) * 100) / dTotal;
                    m_cpuBar->setValue(cpuPct);
                    m_cpuLabel->setText(QString("%1%").arg(cpuPct));
                }
                lastTotal = total;
                lastIdle = idle;
            }
        }

        // Read Uptime
        QFile upFile("/proc/uptime");
        if (upFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            double upSec = QTextStream(&upFile).readLine().split(' ')[0].toDouble();
            m_infoLabel->setText(QString("System Uptime: %1 hours, %2 mins | Monitored via Linux Kernel /proc")
                                 .arg((int)(upSec / 3600)).arg((int)(upSec / 60) % 60));
        }

        // Read Processes
        QDir procDir("/proc");
        QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        m_procTable->setRowCount(0);
        int row = 0;
        for (const QString &entry : entries) {
            bool ok;
            int pid = entry.toInt(&ok);
            if (!ok || row >= 50) continue;

            QFile comm(QString("/proc/%1/comm").arg(pid));
            QFile statm(QString("/proc/%1/statm").arg(pid));
            if (comm.open(QIODevice::ReadOnly | QIODevice::Text) && statm.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString name = QTextStream(&comm).readLine();
                QString rssPages = QTextStream(&statm).readLine().split(' ')[1];
                quint64 rssKb = rssPages.toULongLong() * 4;

                m_procTable->insertRow(row);
                m_procTable->setItem(row, 0, new QTableWidgetItem(QString::number(pid)));
                m_procTable->setItem(row, 1, new QTableWidgetItem(name));
                m_procTable->setItem(row, 2, new QTableWidgetItem("Running"));
                m_procTable->setItem(row, 3, new QTableWidgetItem(QString("%1 KB").arg(rssKb)));
                row++;
            }
        }
    }

    QProgressBar *m_cpuBar;
    QLabel *m_cpuLabel;
    QProgressBar *m_ramBar;
    QLabel *m_ramLabel;
    QProgressBar *m_swapBar;
    QLabel *m_swapLabel;
    QLabel *m_infoLabel;
    QTableWidget *m_procTable;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SystemMonitorWindow win;
    win.show();
    return app.exec();
}
