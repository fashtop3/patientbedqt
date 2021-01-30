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

    setNotificationColor(Qt::lightGray, ui->info_bed_1);
    setNotificationColor(Qt::lightGray, ui->info_bed_2);
    setNotificationColor(Qt::lightGray, ui->info_bed_3);
    setNotificationColor(Qt::lightGray, ui->info_bed_4);

    //    connect(bedTimer, &QTimer::timeout, this, [=](){
    //        qDebug() << "timed out";
    //    });

    setSoundEffects();
    connectInfoLabels();


    bedSubscription(PatientBed::Bed1, Action::Attention);
    //    bedSubscription(PatientBed::Bed2, Action::Clear);
    //    bedSubscription(PatientBed::Bed3, Action::Clear);
    //    bedSubscription(PatientBed::Bed4, Action::Clear);

//    thread = new QThread;
    process = new QProcess(this);

    QString program  = "C:\\Program Files (x86)\\mosquitto\\mosquitto";
    QStringList arguments;
    arguments <<  "-c" << "C:\\Program Files (x86)\\mosquitto\\mosquitto.conf" << "-v";


    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(readOutput(/*int, QProcess::ExitStatus*/)));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readOutput(/*int, QProcess::ExitStatus*/)));

    process->start(program, arguments);

}

void MainWindow::readOutput(/*int exitCode, QProcess::ExitStatus exitStatus*/){
    QString msg = process->readAllStandardError().trimmed();
    qDebug() << "stderr: " << msg;

    ui->textEdit->append(msg.append("\r\n"));
}

MainWindow::~MainWindow()
{
    process->terminate();
    delete ui;
}

void MainWindow::on_pushButtonServer_toggled(bool checked)
{
    qDebug() << "server: " << checked;

    if (!checked) {
        // remove subscription
        ui->pushButtonSubscribe->setChecked(false);
    }
}

void MainWindow::on_pushButtonSubscribe_toggled(bool checked)
{
    qDebug() << "Subscribe: " << checked;
}

void MainWindow::on_pushButtonQuit_clicked()
{

}


void MainWindow::on_pushButtonClear_toggled(bool checked)
{

}

void MainWindow::bedSubscription(PatientBed bed, Action action)
{

    switch (bed) {
    case MainWindow::Bed1:
        bed1Action = action;
        break;

    case MainWindow::Bed2:
        bed2Action = action;
        break;

    case MainWindow::Bed3:
        bed3Action = action;
        break;

    case MainWindow::Bed4:
        bed4Action = action;
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
            bedColorRef = Qt::gray;
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

        this->setInfoToggler(bed2Action, bed2Color, Action::Attention, Qt::yellow, ui->info_bed_2);
        this->setInfoToggler(bed2Action, bed2Color, Action::Distress, Qt::blue, ui->info_bed_2);

        this->setInfoToggler(bed3Action, bed3Color, Action::Attention, Qt::yellow, ui->info_bed_3);
        this->setInfoToggler(bed3Action, bed3Color, Action::Distress, Qt::blue, ui->info_bed_3);

        this->setInfoToggler(bed4Action, bed4Color, Action::Attention, Qt::yellow, ui->info_bed_4);
        this->setInfoToggler(bed4Action, bed4Color, Action::Distress, Qt::blue, ui->info_bed_4);
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
    painter.drawEllipse(15, 20, label->height()/2, label->height()/2);

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
