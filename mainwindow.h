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

    void on_pushButtonClear_toggled(bool checked);

private:
    void setSoundEffects();

    Ui::MainWindow *ui;
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
