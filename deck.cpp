#include "deck.h"

#include <QTime>

Deck::Deck(unsigned size) : list()
{
    initialize(size);
}

Deck::~Deck()
{

}

void Deck::initialize(unsigned size)
{
    for (unsigned i = 1; i <= size; i++)
        list.append(i);
}

int Deck::draw()
{
    qsrand(QTime::currentTime().msecsSinceStartOfDay());
    return list.takeAt(qrand() % list.size());
}

unsigned Deck::size() const
{
    return list.size();
}

