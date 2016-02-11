#include "timeline.h"

TimeLine::TimeLine() : timer(), list(), cur(-1), loop(0)
{
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(next()));
    connect(&timer, SIGNAL(timeout()), this, SIGNAL(timeout()));

    connect(&clicker, SIGNAL(timeout()), this, SIGNAL(click()));
}

TimeLine::~TimeLine()
{

}

bool TimeLine::isRunning() const
{
    return timer.isActive();
}

int TimeLine::remainingMsecs() const
{
    return timer.remainingTime();
}

int TimeLine::currentInterval() const
{
    return timer.interval();
}

QString TimeLine::currentName() const
{
    if (cur == -1)
        return "NOT_IN_PROGRESS";
    return list[cur].name;
}

int TimeLine::getLoop() const
{
    return loop;
}

void TimeLine::addEvent(const QString &name, int msecs)
{
    list.append(TimeLineEvent(name, msecs));
}

void TimeLine::reset()
{
    timer.stop();
    loop = 0;
    cur = -1;
}

void TimeLine::start()
{
    if (list.empty() || list.front().msecs == 0)
        return;
    cur = 0;
    timer.start(list[cur].msecs);
    emit enterEvent(list[cur].name);

    clicker.start(5000);
}

void TimeLine::next()
{
    emit leaveEvent(list[cur].name);
    timer.stop();
    cur++;
    if (cur == list.size())
    {
        cur = 0;
        loop++;
        emit newLoop();
    }
    timer.start(list[cur].msecs);
    emit enterEvent(list[cur].name);
}

void TimeLine::stop()
{
    reset();
}





TimeLineEvent::TimeLineEvent(const QString &name, int msecs) :
    name(name), msecs(msecs)
{

}
