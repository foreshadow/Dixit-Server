#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "tcpsocket.h"

class Player : public QObject
{
    Q_OBJECT
public:
    enum class Color
    {
        Black,
        Blue,
        Green,
        White,
        Yellow,
        Pink
    };

    Player(QString id, TcpSocket *socket);
    ~Player();

    int getScore() const;
    void setScore(int value);
    void addScore(int delta);

    TcpSocket *getSocket() const;

    QString getId() const;
    void setId(const QString &value);

    bool getReady() const;
    void setReady(bool value = true);

    Color getColor() const;
    void setColor(const Color &value);

protected:
    QString id;
    Color color;
    bool ready;
    int score;
    TcpSocket *socket;

signals:
    void updated();
};

#endif // PLAYER_H
