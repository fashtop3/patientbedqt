#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
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
        Attention,
        Distress,
        Clear
    };

    enum PatientBed {
        Bed1,
        Bed2,
        Bed3,
        Bed4
    };


public slots:

    void bedSubscription(PatientBed bed, Action action);

    void connectInfoLabels();

    void bed3Subscription(Action action);

    void bed4Subscription(Action action);

protected:
    void setNotificationColor(QColor color, QLabel *label);

    void setInfoToggler(QColor& bedColor, QColor color, QLabel *label);

private slots:
    void on_pushButtonServer_toggled(bool checked);

    void on_pushButtonSubscribe_toggled(bool checked);

    void on_pushButtonQuit_clicked();

    void on_pushButtonClear_toggled(bool checked);

private:
    void setSoundEffects();

    Ui::MainWindow *ui;
    QSoundEffect *distress;
    QSoundEffect *alert;
    QSoundEffect *clear;

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
