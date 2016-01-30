#ifndef PLAYERLIST_H
#define PLAYERLIST_H

#include <QtCore>
#include <QObject>
#include "player.h"

class PlayerList : public QObject
{
    Q_OBJECT
public:
    PlayerList();
    ~PlayerList();

    int size() const;
    bool allReady() const;
    QList<Player *> &getList();
    void addPlayer(Player *p);
    Player *findPlayer(TcpSocket *socket);
    Player *findPlayer(QString id);

protected:
    QList<Player *> list;

signals:
    void updated();
};

#endif // PLAYERLIST_H
