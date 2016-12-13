#ifndef NIXTRAYWIDGET_H
#define NIXTRAYWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QUrl>
#include <QSslError>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>

#include <QDebug>

class QSslError;

QT_USE_NAMESPACE

/*
namespace Ui {
class NixTrayWidget;
}
*/

class NixTrayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NixTrayWidget(QWidget *parent = 0);

    QNetworkAccessManager manager;
    QList<QNetworkReply *> currentDownloads;
    void doDownload(const QUrl &url);

    ~NixTrayWidget();

private:
  //  Ui::NixTrayWidget *ui;
    QSystemTrayIcon *systray;

    QUrl CHANNEL_AGE_JSON;
    QString CHANNEL;

    QString program;
    QStringList arguments;

    QMenu *channelMenu;
    QAction *unstableAction;
    QAction *stableAction;
    QActionGroup *channelGroup;

    QString mOutputString;
    QString currentRevision;
    QString availableRevision;
    QString lastUpdate;

    QProcess *nixosVersionCmd;

public slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void execute();
    void createMenu();
    void sslErrors(const QList<QSslError> &errors);
    void downloadFinished(QNetworkReply *reply);
    void readyReadStandardOutput();
    void cmdFinished();
};

#endif // NIXTRAYWIDGET_H
