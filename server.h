#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include "tcpserver.h"
#include "playerlist.h"
#include "serverdata.h"
#include "clientdata.h"
#include "timeline.h"
#include "declarer.h"
#include "deck.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

    void listen(int port);

    void sendServerData(TcpSocket *socket, ServerData sd);
    void sendServerDataToAll(ServerData sd);
    void handle(TcpSocket *socket, const ClientData &cd);
    void gameStart();
private:
    static void info(QString message);

protected:
    enum class Status
    {
        BEFORE_GAME_WAITING,
        IN_GAME_DESCRIBING,
        IN_GAME_PLAYING,
        IN_GAME_SELECTING,
        IN_GAME_SETTLING,
        AFTER_GAME
    };

    Status status;
    PlayerList playerList;
    Declarer declarer;

    TcpServer *server;
    TimeLine timeline;
    Deck deck;

signals:

public slots:
    void newClient(TcpSocket *socket);
    void received(TcpSocket *socket, QByteArray message);
    void received(QVariant id, QByteArray message);
    void enterStage(QString stage);
};

#endif // SERVER_H
