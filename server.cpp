#include "server.h"

#include <QtNetwork>
#include <iostream>
#include <numeric>
#include <vector>

#include "serverdata.h"
#include "clientdata.h"

Server::Server(QObject *parent) :
    QObject(parent), status(Status::BEFORE_GAME_WAITING),
    server(new TcpServer), timeline(), deck()
{
    timeline.addEvent("DESCRIBE", 45000);
    timeline.addEvent("PLAY"    , 45000);
    timeline.addEvent("SELECT"  , 45000);
    timeline.addEvent("SETTLE"  , 15000);
    connect(&timeline, SIGNAL(enterEvent(QString)), this, SLOT(enterStage(QString)));
    connect(server, SIGNAL(newClientConnected(TcpSocket *)), this, SLOT(newClient(TcpSocket*)));
    connect(server, SIGNAL(receivedFrom(TcpSocket *, QByteArray)), this, SLOT(received(TcpSocket *, QByteArray)));
    connect(server, SIGNAL(receivedFrom(QVariant, QByteArray)), this, SLOT(received(QVariant, QByteArray)));
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

void Server::sendServerData(TcpSocket *socket, ServerData sd)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_4);
    ds << sd;
    socket->send(ba);
}

void Server::newClient(TcpSocket *socket)
{
    Q_UNUSED(socket);
    info("New connection");
}

void Server::sendServerDataToAll(ServerData sd)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_4);
    ds << sd;
    server->sendToAll(ba);
}

void Server::gameStart()
{
    declarer.shuffle();
    if (status != Status::BEFORE_GAME_WAITING)
        return;
    info("Drawing cards");
    deck.initialize();
    for (int i = 0; i < playerList.size(); i++)
    {
        QList<int> list;
        for (int j = 0; j < 6; j++)
            list.append(deck.draw());
        sendServerData(playerList.getList().at(i)->getSocket(), ServerData(ServerData::DRAW, list));
    }
    info("Game start");
    timeline.start();
}

void Server::handle(TcpSocket *socket, const ClientData &cd)
{
    switch (cd.getType())
    {
    case ClientData::CHAT:
        sendServerDataToAll(ServerData(ServerData::CHAT, cd.getFromUser(), cd.getContent()));
        break;
    case ClientData::PHRASE:
        sendServerDataToAll(ServerData(ServerData::PHRASE, cd.getFromUser(), cd.getContent()));
        break;
    case ClientData::SET_ID:
        // not used
        break;
    case ClientData::READY:
        playerList.addPlayer(new Player(cd.getFromUser(), socket));
        declarer.add(cd.getFromUser());
        info(cd.getFromUser() + " is ready.");
        sendServerDataToAll(ServerData(ServerData::READY, cd.getFromUser()));
        if (status == Status::BEFORE_GAME_WAITING && playerList.size() >= 6)
            gameStart();
        break;
    case ClientData::DESC:
        sendServerDataToAll(ServerData(ServerData::DESC, cd.getFromUser(), cd.getContent()));
        break;
    case ClientData::PLAY:
        sendServerDataToAll(ServerData(ServerData::PLAY, cd.getFromUser(), cd.getContent(), cd.getCards()));
        break;
    case ClientData::SYNC:
        // to do
        break;
    case ClientData::SELECT:
        sendServerDataToAll(ServerData(ServerData::SELECT, cd.getFromUser(), cd.getContent(), cd.getCards()));
        break;
    }
}

void Server::received(TcpSocket *socket, QByteArray message)
{
    handle(socket, ClientData(QDataStream(message)));
}

void Server::received(QVariant id, QByteArray message)
{
    // not used
    Q_UNUSED(id);
    Q_UNUSED(message);
}

void Server::enterStage(QString stage)
{
    info(QString("Entering stage %1, ends at %2").arg(stage)
         .arg(QTime::currentTime().addMSecs(timeline.remainingMsecs()).toString()));
    if (stage == "DESCRIBE")
    {
        status = Status::IN_GAME_DESCRIBING;
        sendServerDataToAll(ServerData(ServerData::SYNC, declarer.next(), "DESC"));
    }
    else if (stage == "PLAY")
    {
        status = Status::IN_GAME_PLAYING;
        sendServerDataToAll(ServerData(ServerData::SYNC, declarer.current(), "PLAY"));
    }
    else if (stage == "SELECT")
    {
        status = Status::IN_GAME_SELECTING;
        sendServerDataToAll(ServerData(ServerData::SYNC, declarer.current(), "SELECT"));
    }
    else if (stage == "SETTLE")
    {
        status = Status::IN_GAME_SETTLING;
        sendServerDataToAll(ServerData(ServerData::SYNC, declarer.current(), "SETTLE"));
    }
}

