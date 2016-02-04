#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include "tcpserver.h"
#include "serverdata.h"
#include "clientdata.h"
#include "dixitgame.h"
#include "timeline.h"
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
    QList<Player> playerList;
    DixitGame *dixit;
    TcpServer *server;
    TimeLine timeline;
    Deck deck;

signals:

public slots:
    void newClient(TcpSocket *socket);
    void received(TcpSocket *socket, QByteArray message);
    void received(QVariant id, QByteArray message);
    void enterStage(QString stage);
    void message(QString message);
    void descFin();
    void playFin();
    void seleFin();
    void settFin();
};

#endif // SERVER_H
