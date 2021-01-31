#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBitmap>
#include <QDebug>
#include <QImageReader>
#include <QPainter>
#include <QSound>
#include <QAudioDeviceInfo>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSoundEffect>
#include <QTimer>
#include <QTextCodec>
#include <QDateTime>



void MainWindow::clearAllInfo()
{
    bed1Action = Action::Clear; //bed1Color = Qt::transparent;
    bed2Action = Action::Clear;// bed2Color = Qt::transparent;
    bed3Action = Action::Clear; //bed3Color = Qt::transparent;
    bed4Action = Action::Clear; //bed4Color = Qt::transparent;

    setNotificationColor(Qt::lightGray, ui->info_bed_1);
    setNotificationColor(Qt::lightGray, ui->info_bed_2);
    setNotificationColor(Qt::lightGray, ui->info_bed_3);
    setNotificationColor(Qt::lightGray, ui->info_bed_4);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bedTimer = new QTimer();
    bedTimer->setInterval(500);
    bedTimer->start();

    ui->pushButtonClear->setDefault(false);
    ui->pushButtonServer->setDefault(false);
    ui->pushButtonSubscribe->setDefault(false);
    ui->pushButtonSubscribe->setEnabled(false);

    ui->comboBox->addItems(QStringList()
                           << "---"
                           << "bed01"
                           << "bed02"
                           << "bed03"
                           << "bed04");

    clearAllInfo();


    setSoundEffects();
    connectInfoLabels();


    setNotificationColor(Qt::red, ui->conn_bed_1);
    setNotificationColor(Qt::red, ui->conn_bed_2);
    setNotificationColor(Qt::red, ui->conn_bed_3);
    setNotificationColor(Qt::red, ui->conn_bed_4);

    actionMap["attention"] = Action::Attention;
    actionMap["distress"] = Action::Distress;
    actionMap["clear"] = Action::Clear;

    bedMap["bed01"] = PatientBed::Bed1;
    bedMap["bed02"] = PatientBed::Bed2;
    bedMap["bed03"] = PatientBed::Bed3;
    bedMap["bed04"] = PatientBed::Bed4;

    connPixmap["bed01"] = ui->conn_bed_1;
    connPixmap["bed02"] = ui->conn_bed_2;
    connPixmap["bed03"] = ui->conn_bed_3;
    connPixmap["bed04"] = ui->conn_bed_4;

    serverProcess = new QProcess(this);
    subscribeProcess = new QProcess(this);
    publishProcess = new QProcess(this);

    connect(serverProcess, &QProcess::readyReadStandardError,
            this, &MainWindow::readOutput);

    connect(subscribeProcess, &QProcess::readyRead,
            this, &MainWindow::readMessageSubscription);

    connect(publishProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::publishMessageFinished);


    this->setWindowTitle("PATIENT BED MONITOR");

//    qDebug() << "App path : " << qApp->applicationDirPath() + "/mosquitto";

    mosquito_path = qApp->applicationDirPath() + "/mosquitto";

}

void MainWindow::readOutput(/*int exitCode, QProcess::ExitStatus exitStatus*/){
    QString msg = serverProcess->readAllStandardError().trimmed();
    qDebug() << "stderr: " << msg;

    //check if connect bit is set else set it
    if (msg.contains("Sending SUBACK to ")) {
        QString client_id = msg.split(" ").last();
        if (connPixmap[client_id])
            setNotificationColor(Qt::green, connPixmap[client_id]);
    }

//    1612047970: Client bed04 has exceeded timeout, disconnecting.
    if(msg.contains("has exceeded timeout, disconnecting.")) {
        QString client_id = msg.split(" ")[2];
        setNotificationColor(Qt::red, connPixmap[client_id]);
    }

    ui->textEdit->append(msg.append("\r\n"));
}

void MainWindow::readMessageSubscription()
{
    QString msg = subscribeProcess->readAll().trimmed();
    //    process event here
    QStringList msgSplit = msg.split("/");

    bedSubscription(bedMap[msgSplit[1]], actionMap[msgSplit[0]]);

    qDebug() << "\r\nMessage: " << msg;

    ui->textEdit->append(QString("<font color='orange'>Message:&nbsp;&nbsp;</font>"
                                 " <font color='blue'>\"%1\"</font>\t (%2) <br>")
                         .arg(msg)
                         .arg(QDateTime::currentDateTime().toString()));
}

MainWindow::~MainWindow()
{
    serverProcess->kill();
    ui->textEdit->append("<b><font color='red'>Server stopped.</font></b>");
    delete ui;
}

void MainWindow::on_pushButtonServer_toggled(bool checked)
{
    qDebug() << "server: " << checked;

    if(checked){
        ui->textEdit->append("<b><font color='green'>Starting server.</font></b>\r\n");
        QString program  = mosquito_path + "/mosquitto";
        QStringList arguments;
        arguments <<  "-c" << mosquito_path + "/mosquitto.conf" << "-v";
        serverProcess->start(program, arguments);
    }

    if (!checked) {
        serverProcess->kill();
        ui->textEdit->append("<b><font color='red'>Server stopped.</font></b>");
        // remove subscription
        ui->pushButtonSubscribe->setChecked(false);
    }
}

void MainWindow::on_pushButtonSubscribe_toggled(bool checked)
{
    qDebug() << "Subscribe: " << checked;
    if(checked){
        ui->textEdit->append("<b><font color='green'>subcribing to: <i>monitor/patient/bed</i></font></b>\r\n");
        QString program  = mosquito_path + "/mosquitto_sub.exe";
        QStringList arguments;
        arguments <<  "-t" << "monitor/patient/bed";
        subscribeProcess->start(program, arguments);
    }

    if (!checked) {
        subscribeProcess->kill();
        ui->textEdit->append("<b><font color='red'>unsubscribed from: <i>monitor/patient/bed</i></font></b>");
        // remove subscription
        ui->pushButtonSubscribe->setChecked(false);
        clearAllInfo();
        attentionEffect->stop();
        clearEffect->stop();
        distressEffect->stop();
    }
}

void MainWindow::on_pushButtonQuit_clicked()
{
    serverProcess->kill();
    subscribeProcess->kill();
}


void MainWindow::bedSubscription(PatientBed bed, Action action)
{

    switch (bed) {
    case MainWindow::Bed1:
        bed1Action = action;
        if(action == Action::Clear)
            setNotificationColor(Qt::green, ui->info_bed_1);
        break;

    case MainWindow::Bed2:
        bed2Action = action;
        if(action == Action::Clear)
            setNotificationColor(Qt::green, ui->info_bed_2);
        break;

    case MainWindow::Bed3:
        bed3Action = action;
        if(action == Action::Clear)
            setNotificationColor(Qt::green, ui->info_bed_3);
        break;

    case MainWindow::Bed4:
        bed4Action = action;
        if(action == Action::Clear)
            setNotificationColor(Qt::green, ui->info_bed_4);
        break;
    }

    int checkBit = bed1Action | bed2Action | bed3Action | bed4Action;

    if(checkBit & (1<<0))
        attentionEffect->play();
    else
        attentionEffect->stop();

    if(checkBit & (1<<1))
        distressEffect->play();
    else
        distressEffect->stop();

}

void MainWindow::setInfoToggler(Action& bedActionRef,
                                QColor& bedColorRef,
                                Action actionCheck,
                                QColor color,
                                QLabel *labelRef)
{
    if(bedActionRef != Action::Clear && bedActionRef == actionCheck) {
        //        qDebug() << bedActionRef << bedColorRef << color;

        if (bedColorRef == color)
            bedColorRef = Qt::lightGray;
        else
            bedColorRef = color;

        this->setNotificationColor(bedColorRef, labelRef);
    }

}

void MainWindow::connectInfoLabels()
{
    bed1Action = Action::Clear; bed1Color = Qt::transparent;
    bed2Action = Action::Clear; bed2Color = Qt::transparent;
    bed3Action = Action::Clear; bed3Color = Qt::transparent;
    bed4Action = Action::Clear; bed4Color = Qt::transparent;

    connect(bedTimer, &QTimer::timeout, this, [=]() {
        this->setInfoToggler(bed1Action, bed1Color, Action::Attention, Qt::yellow, ui->info_bed_1);
        this->setInfoToggler(bed1Action, bed1Color, Action::Distress, Qt::blue, ui->info_bed_1);
        this->setInfoToggler(bed1Action, bed1Color, Action::Clear, Qt::gray, ui->info_bed_1);

        this->setInfoToggler(bed2Action, bed2Color, Action::Attention, Qt::yellow, ui->info_bed_2);
        this->setInfoToggler(bed2Action, bed2Color, Action::Distress, Qt::blue, ui->info_bed_2);
        this->setInfoToggler(bed1Action, bed1Color, Action::Clear, Qt::gray, ui->info_bed_2);

        this->setInfoToggler(bed3Action, bed3Color, Action::Attention, Qt::yellow, ui->info_bed_3);
        this->setInfoToggler(bed3Action, bed3Color, Action::Distress, Qt::blue, ui->info_bed_3);
        this->setInfoToggler(bed1Action, bed1Color, Action::Clear, Qt::gray, ui->info_bed_3);

        this->setInfoToggler(bed4Action, bed4Color, Action::Attention, Qt::yellow, ui->info_bed_4);
        this->setInfoToggler(bed4Action, bed4Color, Action::Distress, Qt::blue, ui->info_bed_4);
        this->setInfoToggler(bed1Action, bed1Color, Action::Clear, Qt::gray, ui->info_bed_4);
    });
}


void MainWindow::setNotificationColor(QColor color, QLabel *label)
{
    QPixmap target = QPixmap(label->size());
    target.fill(Qt::transparent);

    QPainter painter (&target);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(12, 20, label->height()/2, label->height()/2);

    label->setPixmap(target);
}

void MainWindow::setSoundEffects()
{
    attentionEffect = new QSoundEffect();
    attentionEffect->setSource(QUrl("qrc:/sounds/alert.wav"));
    attentionEffect->setLoopCount(QSoundEffect::Infinite);
    attentionEffect->setVolume(1.0);

    distressEffect = new QSoundEffect();
    distressEffect->setSource(QUrl("qrc:/sounds/distress.wav"));
    distressEffect->setLoopCount(QSoundEffect::Infinite);
    distressEffect->setVolume(1.0);

    clearEffect = new QSoundEffect();
    clearEffect->setSource(QUrl("qrc:/sounds/clear.wav"));
    clearEffect->setVolume(1.0);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->pushButtonClear->setEnabled(index > 0);
}

void MainWindow::on_pushButtonClear_clicked()
{
    //send a message to a reply channel
    QString program  = mosquito_path.append("/mosquitto_pub.exe");
    QStringList arguments;
    arguments <<  "-t" << "reply/patient/bed" << "-m" << QString("clear/").append(ui->comboBox->currentText());
    publishProcess->start(program, arguments);
    bedSubscription(bedMap[ui->comboBox->currentText()], Action::Clear);

}

void MainWindow::publishMessageFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << exitCode << exitStatus;
    if (exitStatus == QProcess::NormalExit) {
        ui->textEdit->append(QString("<font color='orange'>Command sent:&nbsp;&nbsp;</font>"
                                     " <font color='blue'>\"%1\"</font>\t (%2) <br>")
                             .arg(QString("clear/").append(ui->comboBox->currentText()))
                             .arg(QDateTime::currentDateTime().toString()));
        ui->comboBox->setCurrentIndex(0);
    }
}
