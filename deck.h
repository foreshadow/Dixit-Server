#ifndef DECK_H
#define DECK_H

#include <QObject>

class Deck : public QObject
{
    Q_OBJECT
public:
    explicit Deck(unsigned size = 84);
    ~Deck();

    void initialize(unsigned size = 84);
    int draw();
    unsigned size() const;

protected:
    QList<int> list;

signals:

public slots:
};

#endif // DECK_H
