#ifndef DECLARER_H
#define DECLARER_H

#include <QObject>

class Declarer : public QObject
{
    Q_OBJECT
public:
    Declarer();
    ~Declarer();

    void add(const QString& name);
    void shuffle();
    void reset();
    int size() const;
    QList<QString> &getList();
    QString next();
    QString current() const;

protected:
    QList<QString> list;
    int i;

signals:

public slots:
};

#endif // DECLARER_H
