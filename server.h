#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include "tcpserver.h"
#include "playerlist.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

    void listen(int port);

private:
    static void info(QString message);

    enum class Status
    {
        BEFORE_GAME_WAITING,
        IN_GAME_PREPARING,
        IN_GAME_DESCRIBING,
        IN_GAME_DEALING_CARDS,
        IN_GAME_SETTLING,
        AFTER_GAME
    };

    Status status;
    PlayerList playerList;

protected:
    TcpServer *server;

signals:

public slots:
    void newClient(TcpSocket *socket);
    void received(TcpSocket *socket, QString message);
    void received(QVariant id, QString message);
};

#endif // SERVER_H
