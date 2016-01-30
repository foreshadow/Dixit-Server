#include "declarer.h"

#include <algorithm>

Declarer::Declarer()
{

}

Declarer::~Declarer()
{

}

void Declarer::add(const QString &name)
{
    list.append(name);
}

void Declarer::shuffle()
{
    std::random_shuffle(list.begin(), list.end());
}

void Declarer::reset()
{
    i = -1;
}

int Declarer::size() const
{
    return list.size();
}

QList<QString> &Declarer::getList()
{
    return list;
}

QString Declarer::next()
{
    i = (i + 1) % list.size();
    return current();
}

QString Declarer::current() const
{
    return list.at(i);
}

