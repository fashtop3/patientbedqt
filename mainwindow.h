#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serverthread.h"

#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QSound>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum Action {
        Clear = 0,
        Attention,
        Distress
    };

    enum PatientBed {
        Bed1 = 1,
        Bed2 = 2,
        Bed3 = 3,
        Bed4 = 4
    };


    void clearAllInfo();

public slots:

    void bedSubscription(PatientBed bed, Action action);

    void connectInfoLabels();

protected:
    void setNotificationColor(QColor color, QLabel *label);

    void setInfoToggler(Action &bedActionRef,
                        QColor& bedColorRef, Action actionCheck,
                        QColor color,
                        QLabel *labelRef);

private slots:
    void on_pushButtonServer_toggled(bool checked);

    void on_pushButtonSubscribe_toggled(bool checked);

    void on_pushButtonQuit_clicked();

    void readOutput();

    void readMessageSubscription();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButtonClear_clicked();

    void publishMessageFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void setSoundEffects();

    Ui::MainWindow *ui;
    //    ServerThread serverThread;
    QProcess *serverProcess;
    QProcess *subscribeProcess;
    QProcess *publishProcess;

    QMap<QString, Action> actionMap;
    QMap<QString, PatientBed> bedMap;

    QSoundEffect *distressEffect;
    QSoundEffect *attentionEffect;
    QSoundEffect *clearEffect;

    QTimer *bedTimer;

    QColor bed1Color;
    Action bed1Action;

    QColor bed2Color;
    Action bed2Action;

    QColor bed3Color;
    Action bed3Action;

    QColor bed4Color;
    Action bed4Action;
};

#endif // MAINWINDOW_H
