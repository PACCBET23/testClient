#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QThread>
#include <QUdpSocket>

class NetworkThread : public QThread
{
    Q_OBJECT

public:
    explicit NetworkThread(QObject *parent = nullptr);
    ~NetworkThread();
    void setupSocket(quint16 port);

signals:
    void dataReceived(const QByteArray &data);

protected:
    void run() override;

private:
    quint16 port;
};

#endif // NETWORKTHREAD_H
