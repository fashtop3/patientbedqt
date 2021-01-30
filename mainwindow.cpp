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



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bedTimer = new QTimer();
    bedTimer->setInterval(800);
    bedTimer->start();

    ui->pushButtonClear->setDefault(false);
    ui->pushButtonServer->setDefault(false);
    ui->pushButtonSubscribe->setDefault(false);
    ui->pushButtonSubscribe->setEnabled(false);

    setNotificationColor(Qt::red, ui->info_bed_1);
    setNotificationColor(Qt::blue, ui->info_bed_2);
    setNotificationColor(Qt::green, ui->info_bed_3);
    setNotificationColor(Qt::lightGray, ui->info_bed_4);

    connect(bedTimer, &QTimer::timeout, this, [=](){
        qDebug() << "timed out";
    });

    setSoundEffects();

    //    alert->play();
    //    distress->play();
}

MainWindow::~MainWindow()
{
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
}

void MainWindow::setInfoToggler(Action& bedAction,
                                QColor& bedColor,
                                QColor color,
                                QLabel *label)
{
    if(bedAction != Action::Clear) {
        bedColor = color;
        if (bedColor == color)
            bedColor = Qt::gray;
        else
            bedColor = color;

        this->setNotificationColor(bedColor, label);
    }
    //    bed1Color = Qt::blue;
    //    if (bed1Color == Qt::blue)
    //        bed1Color = Qt::gray;
    //    else
    //        bed1Color = Qt::yellow;

    //    this->setNotificationColor(bed1Color, ui->info_bed_4);
}

void MainWindow::connectInfoLabels()
{
    //bed 1
    connect(bedTimer, &QTimer::timeout, this, [=]() {
        this->setInfoToggler(bed1Action, bed1Color, Qt::yellow, ui->info_bed_1);
        this->setInfoToggler(bed1Action, bed1Color, Qt::blue, ui->info_bed_1);

        this->setInfoToggler(bed2Action, bed2Color, Qt::yellow, ui->info_bed_2);
        this->setInfoToggler(bed2Action, bed2Color, Qt::blue, ui->info_bed_2);

        this->setInfoToggler(bed3Action, bed3Color, Qt::yellow, ui->info_bed_3);
        this->setInfoToggler(bed3Action, bed3Color, Qt::blue, ui->info_bed_3);

        this->setInfoToggler(bed4Action, bed4Color, Qt::yellow, ui->info_bed_4);
        this->setInfoToggler(bed4Action, bed4Color, Qt::blue, ui->info_bed_4);
    });
}

void MainWindow::bed3Subscription(Action action)
{

}

void MainWindow::bed4Subscription(Action action)
{

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
    alert = new QSoundEffect();
    alert->setSource(QUrl("qrc:/sounds/alert.wav"));
    alert->setLoopCount(QSoundEffect::Infinite);
    alert->setVolume(1.0);

    distress = new QSoundEffect();
    distress->setSource(QUrl("qrc:/sounds/distress.wav"));
    distress->setLoopCount(QSoundEffect::Infinite);
    distress->setVolume(1.0);

    clear = new QSoundEffect();
    clear->setSource(QUrl("qrc:/sounds/clear.wav"));
    clear->setVolume(1.0);
}
