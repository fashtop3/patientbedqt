#include "serverthread.h"
#include <QDebug>

ServerThread::ServerThread()
{
    program  = "C:\\Program Files (x86)\\mosquitto\\mosquitto";
    arguments << "-c" << ".\\mosquitto.conf" << "-v";

}

void ServerThread::terminate()
{
    process->terminate();
    qDebug() << "Process terminated.";
    QThread::terminate();
}

void ServerThread::run()
{
    QProcess* process = new QProcess();
    connect(process, &QProcess::readyReadStandardOutput, this, [=](){
        QString msg = process->readAll().trimmed();
        qDebug() << "Standard output ready: " << msg;
       emit this->sendOutput(msg);
    });

    process->start(program, arguments);
}
