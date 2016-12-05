#include "nixtraywidget.h"

NixTrayWidget::NixTrayWidget(QWidget *parent) :
    QWidget(parent)
  //,  ui(new Ui::NixTrayWidget)
{
    //ui->setupUi(this);
    systray = new QSystemTrayIcon(this);
    systray->setIcon(QIcon::fromTheme("update-none"));
    systray->show();
    systray->setToolTip("Checking...");

    CHANNEL_AGE_JSON = "https://howoldis.herokuapp.com/api/channels";

    QSettings settings("NixOS", "Nix Tray");

    CHANNEL = settings.value("channel", "nixos-unstable").toString();

    program = "nixos-version";
    arguments << "--revision";

    lastUpdate = "";
    availableRevision = "";

    nixosVersionCmd = new QProcess(this);
    connect(nixosVersionCmd, SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(nixosVersionCmd, SIGNAL(finished(int)), this, SLOT(cmdFinished()));

    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));

    // Run once
    execute();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(execute()));
    timer->start(1000 * 60 * 30);

    // Left click
    connect(systray, &QSystemTrayIcon::activated, this, &NixTrayWidget::onActivated);


}

void NixTrayWidget::execute()
{
    nixosVersionCmd->start(program, arguments);
}

void NixTrayWidget::readyReadStandardOutput()
{
    mOutputString.append(nixosVersionCmd->readAllStandardOutput());
}

void NixTrayWidget::cmdFinished()
{
    currentRevision = mOutputString.left(7);
    doDownload(CHANNEL_AGE_JSON);
}

void NixTrayWidget::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloads.append(reply);
}

void NixTrayWidget::downloadFinished(QNetworkReply *reply)
{
    QByteArray* buffer = new QByteArray();
    buffer->append(reply->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(*buffer);
    QJsonArray array = doc.array();
    for (int i = 0; i <= array.size(); i++) {
        QJsonObject obj = array[i].toObject();
        if (obj["name"] == CHANNEL) {
            lastUpdate = obj["humantime"].toString();
            availableRevision = obj["commit"].toString();
            if (currentRevision == availableRevision) {
                systray->setToolTip("Up to date\n" + CHANNEL + " channel age: " + lastUpdate);
                systray->setIcon(QIcon::fromTheme("update-none"));
            } else {
                systray->setToolTip(CHANNEL + " update available: " + lastUpdate);
                systray->setIcon(QIcon::fromTheme("update-low"));
            }
        }
    }
}

void NixTrayWidget::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (currentRevision == "" or availableRevision == "") {
            // do nothing
        } else if (currentRevision != availableRevision) {
            QString revisions = currentRevision + "..." + availableRevision;
            QDesktopServices::openUrl(QUrl("https://github.com/NixOS/nixpkgs/compare/" + revisions));
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle("No updates available");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("The last " + CHANNEL + " channel update was "+ lastUpdate +" ago.");
            msgBox.exec();
        }
    }
}

void NixTrayWidget::sslErrors(const QList<QSslError> &errors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, errors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(errors);
#endif
}


NixTrayWidget::~NixTrayWidget()
{
    //delete ui;
}
