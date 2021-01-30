#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QProcess>
#include <QThread>

class ServerThread : public QThread
{
    Q_OBJECT

    signals:
    void sendOutput(QString output);
public:
    ServerThread();

public slots:
    void terminate();


private:
    void run();

    QProcess* process;
    QString program;
    QStringList arguments;
};

#endif // SERVERTHREAD_H
