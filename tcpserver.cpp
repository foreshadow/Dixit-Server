#include "tcpserver.h"

TcpServer::TcpServer() :
    server(new QTcpServer),
    listeningStatus(false),
    socketList()
{
    connect(server, SIGNAL(newConnection()), this, SLOT(newPendingConnection()));
}

TcpServer::~TcpServer()
{
    server->deleteLater();
}

bool TcpServer::isListening() const
{
    return listeningStatus;
}

bool TcpServer::listen(const QHostAddress &address, quint16 port)
{
    if (isListening() == false)
        listeningStatus = server->listen(address, port);
    return isListening();
}

void TcpServer::removeDisconnected()
{
    for (int i = 0; i < socketList.size(); i++)
        if (!socketList.at(i)->isConnected())
            socketList.takeAt(i--);
}

int TcpServer::size()
{
    removeDisconnected();
    return socketList.size();
}

const QList<TcpSocket *> &TcpServer::getSocketList() const
{
    return socketList;
}

void TcpServer::send(QVariant id, QByteArray message)
{
    foreach (TcpSocket *socket, socketList)
    {
        if (socket->getId() == id)
        {
            socket->send(message);
            break;
        }
    }
}

void TcpServer::send(TcpSocket *socket, QByteArray message)
{
    socket->send(message);
}

void TcpServer::sendToAll(QByteArray message)
{
    foreach (TcpSocket *socket, socketList)
    {
        send(socket, message);
    }
}

void TcpServer::newPendingConnection()
{
    TcpSocket *socket = new TcpSocket(server->nextPendingConnection());
    connect(socket, SIGNAL(received(QByteArray)), this, SLOT(received(QByteArray)));
    socketList << socket;
    emit newClientConnected(socket);
}

void TcpServer::received(QByteArray message)
{
    TcpSocket *socket = qobject_cast<TcpSocket *>(sender());
    emit receivedFrom(socket->getId(), message);
    emit receivedFrom(socket, message);
}
