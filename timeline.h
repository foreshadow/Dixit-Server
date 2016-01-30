#ifndef TIMELINE_H
#define TIMELINE_H

#include <QObject>
#include <QTimer>

class TimeLineEvent;

class TimeLine : public QObject
{
    Q_OBJECT

public:
    explicit TimeLine();
    ~TimeLine();

    bool isRunning() const;
    int remainingMsecs() const;
    int currentInterval() const;
    QString currentName() const;
    int getLoop() const;

    void addEvent(const QString &name, int msecs);

protected:
    QTimer timer;
    QList<TimeLineEvent> list;
    int cur;
    int loop;

signals:
    void timeout();
    void newLoop();
    void leaveEvent(QString);
    void enterEvent(QString);

public slots:
    void reset();
    void start();
    void next();
    void stop();
};

class TimeLineEvent
{
public:
    TimeLineEvent(const QString &name, int msecs);
    QString name;
    int msecs;
};

#endif // TIMELINE_H
