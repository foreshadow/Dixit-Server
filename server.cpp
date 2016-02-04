#include "server.h"

#include <QtNetwork>
#include <iostream>
#include <numeric>
#include <vector>

#include "serverdata.h"
#include "clientdata.h"

Server::Server(QObject *parent) :
    QObject(parent), dixit(new DixitGame()),
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
    connect(dixit, SIGNAL(descriptionChanged()), this, SLOT(descFin()));
    connect(dixit, SIGNAL(allPlayed()), this, SLOT(playFin()));
    connect(dixit, SIGNAL(allSelected()), this, SLOT(seleFin()));
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
    for (int i = 0; i < dixit->playerList().size(); i++)
        dixit->playerList()[i].setSeat(i);
    info("Drawing cards");
    deck.initialize();
    for (int i = 0; i < dixit->playerList().size(); i++)
    {
        QList<int> list;
        for (int j = 0; j < 6; j++)
            list.append(deck.draw());
        dixit->playerList()[i].setHandCards(list);
        sendServerData(dixit->playerList()[i].getSocket(), ServerData(ServerData::Type::DRAW, list));
    }
    qsrand(QTime::currentTime().msec());
    dixit->playerList()[qrand() % dixit->playerList().size()].setActive();
    info("Game start");
    timeline.start();
}

void Server::handle(TcpSocket *socket, const ClientData &cd)
{
    switch (cd.getType())
    {
    case ClientData::Type::CHAT:
        sendServerDataToAll(ServerData(ServerData::Type::CHAT, cd.getFromUser(), cd.getContent()));
        break;
    case ClientData::Type::PHRASE:
        sendServerDataToAll(ServerData(ServerData::Type::PHRASE, cd.getFromUser(), cd.getContent()));
        break;
    case ClientData::Type::SET_ID:
        // not used
        break;
    case ClientData::Type::READY:
        if (dixit->status() != DixitGame::Status::DIXIT_BEFORE_GAME)
            break;
        playerList.append(Player(cd.getFromUser(), socket));
        dixit->playerList().append(Player(cd.getFromUser(), socket));
        info(cd.getFromUser() + " is ready.");
        sendServerDataToAll(ServerData(ServerData::Type::SYNC, *dixit));
        sendServerDataToAll(ServerData(ServerData::Type::READY, cd.getFromUser()));
        if (dixit->isPlaying() == false && playerList.size() >= 6)
            gameStart();
        break;
    case ClientData::Type::DESC:
        if (dixit->status() == DixitGame::Status::DIXIT_IN_GAME_DESCRIBING
                && cd.getFromUser() == dixit->currentPlayer()->getId())
        {
            sendServerDataToAll(ServerData(ServerData::Type::DESC, cd.getFromUser(), cd.getContent()));
            dixit->setDescription(cd.getContent());
        }
        break;
    case ClientData::Type::PLAY:
        if (dixit->status() == DixitGame::Status::DIXIT_IN_GAME_PLAYING
                && dixit->findPlayer(socket)->getPlayed() == false)
        {
            sendServerDataToAll(ServerData(ServerData::Type::PLAY, cd.getFromUser(), cd.getContent(), cd.getCards()));
            dixit->findPlayer(socket)->setPlayed();
        }
        break;
    case ClientData::Type::SYNC:
        sendServerData(socket, ServerData(ServerData::Type::SYNC, *dixit));
        break;
    case ClientData::Type::SELECT:
        if (dixit->status() == DixitGame::Status::DIXIT_IN_GAME_SELECTING
                && dixit->findPlayer(socket)->getSelected() == false)
        {
            sendServerDataToAll(ServerData(ServerData::Type::SELECT, cd.getFromUser(), cd.getContent(), cd.getCards()));
            dixit->findPlayer(socket)->setSelected();
        }
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
        int c = dixit->currentPlayer()->getSeat();
        dixit->currentPlayer()->setActive(false);
        c = (c + 1) % dixit->playerList().size();
        dixit->playerList()[c].setActive();
        dixit->setStatus(DixitGame::Status::DIXIT_IN_GAME_DESCRIBING);
        sendServerDataToAll(ServerData(ServerData::Type::SYNC, dixit->currentPlayer()->getId(), "DESC"));
    }
    else if (stage == "PLAY")
    {
        dixit->setStatus(DixitGame::Status::DIXIT_IN_GAME_PLAYING);
        sendServerDataToAll(ServerData(ServerData::Type::SYNC, dixit->currentPlayer()->getId(), "PLAY"));
    }
    else if (stage == "SELECT")
    {
        dixit->setStatus(DixitGame::Status::DIXIT_IN_GAME_SELECTING);
        sendServerDataToAll(ServerData(ServerData::Type::SYNC, dixit->currentPlayer()->getId(), "SELECT"));
    }
    else if (stage == "SETTLE")
    {
        dixit->setStatus(DixitGame::Status::DIXIT_IN_GAME_SETTLING);
        sendServerDataToAll(ServerData(ServerData::Type::SYNC, dixit->currentPlayer()->getId(), "SETTLE"));
    }
}

void Server::message(QString message)
{
    sendServerDataToAll(ServerData(ServerData::Type::CHAT, "SERVER", message));
}

void Server::descFin()
{
    if (timeline.currentName() == "DESCRIBE" && timeline.remainingMsecs() > 1000)
        timeline.next();
}

void Server::playFin()
{
    if (timeline.currentName() == "PLAY" && timeline.remainingMsecs() > 1000)
        timeline.next();
}

void Server::seleFin()
{
    if (timeline.currentName() == "SELECT" && timeline.remainingMsecs() > 1000)
        timeline.next();
}

void Server::settFin()
{

}

