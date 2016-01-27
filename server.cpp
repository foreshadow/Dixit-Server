#include "server.h"

#include <QtNetwork>
#include <iostream>

Server::Server(QObject *parent) :
    QObject(parent),
    server(new TcpServer)
{
    connect(server, SIGNAL(newClientConnected(TcpSocket *)), this, SLOT(newClient(TcpSocket*)));
    connect(server, SIGNAL(receivedFrom(TcpSocket *, QString)), this, SLOT(received(TcpSocket *, QString)));
    connect(server, SIGNAL(receivedFrom(QVariant, QString)), this, SLOT(received(QVariant, QString)));
}

Server::~Server()
{

}

void Server::listen(int port)
{
    bool ok = server->listen(QHostAddress::Any, port);
    info(QString(ok ? "Listening on port %1" : "Listen on port %1 failed.").arg(port));
}

void Server::info(QString message)
{
    std::cout << (QTime::currentTime().toString() + " [INFO] " + message).toStdString() << std::endl;
}

void Server::newClient(TcpSocket *socket)
{
    info("New connection");
    socket->send("/ACCEPT");
}

void Server::received(TcpSocket *socket, QString message) // not completed
{
    info(playerList.findPlayer(socket)->getId() + " : " + message);
    if (message.left(1) == "/")
    {
        QStringList argv = message.right(message.size() - 1).split(" ");
#define ARGC_CHECK(n) if (argv.size() < n) return;
        ARGC_CHECK(1);
        QString &operation = argv[0];
        if (operation == "ID")
        {
            ARGC_CHECK(2);
            if (socket->getId() == "")
                socket->setId(argv[1]);
        }
        else if (operation == "READY")
        {
            if (socket->getId() == "")
            {
                info("Empty id.");
                return;
            }
            if (playerList.findPlayer(socket->getId()))
            {
                info("Duplicate ids : " + socket->getId() + " denying.");
                return;
            }
            playerList.addPlayer(new Player(socket->getId(), socket));
            server->sendToAll(message + " " + socket->getId());
        }
        else if (operation == "DESC")
            server->sendToAll(message);
        else if (operation == "PLAY")
            server->sendToAll("/PLAY " + socket->getId());
        else if (operation == "SYNC")
            ;
        else if (operation == "SELECT")
            server->sendToAll("/SELECT " + socket->getId());
        else if (operation == "PHRASE")
        {
            ARGC_CHECK(2);
            server->sendToAll("/PHRASE " + argv[1] + " " + socket->getId());
        }
#undef ARGC_CHECK
    }
    else
    {
        server->sendToAll(message);
    }
}

void Server::received(QVariant id, QString message)
{
    // not used
    Q_UNUSED(id);
    Q_UNUSED(message);
}

