#include "playerlist.h"

PlayerList::PlayerList()
{

}

PlayerList::~PlayerList()
{

}

int PlayerList::size() const
{
    return list.size();
}

bool PlayerList::allReady() const
{
    for (Player *p : list)
        if (p->getReady() == false)
            return false;
    return true;
}

QList<Player *> &PlayerList::getList()
{
    return list;
}

void PlayerList::addPlayer(Player *p)
{
    list << p;
    connect(p, SIGNAL(updated()), this, SIGNAL(updated()));
}

Player *PlayerList::findPlayer(TcpSocket *socket)
{
    for (int i = 0; i < list.size(); i++)
        if (list[i]->getSocket() == socket)
            return list[i];
    return nullptr;
}

Player *PlayerList::findPlayer(QString id)
{
    for (int i = 0; i < list.size(); i++)
        if (list[i]->getId() == id)
            return list[i];
    return nullptr;
}
