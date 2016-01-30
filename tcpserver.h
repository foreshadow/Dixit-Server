#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpsocket.h"

#include <QtNetwork>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    TcpServer();
    ~TcpServer();

    bool isListening() const;
    bool listen(const QHostAddress &address, quint16 port);

    void removeDisconnected();
    int size();
    const QList<TcpSocket *> &getSocketList() const;

protected:
    QTcpServer *server;
    bool listeningStatus;
    QList<TcpSocket *> socketList;

signals:
    void receivedFrom(TcpSocket *, QByteArray);
    void receivedFrom(QVariant, QByteArray);
    void newClientConnected(TcpSocket *);

public slots:
    void send(QVariant id, QByteArray message);
    void send(TcpSocket *socket, QByteArray message);
    void sendToAll(QByteArray message);

protected slots:
    void newPendingConnection();
    void received(QByteArray message);
};

#endif // TCPSERVER_H
