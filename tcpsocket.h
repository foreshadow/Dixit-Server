#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QtCore>
#include <QtNetwork>

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    TcpSocket();
    TcpSocket(QTcpSocket *socket);
    ~TcpSocket();

    void connectToHost(const QHostAddress &address, quint16 port = 9527,
                       QAbstractSocket::OpenMode mode = QAbstractSocket::ReadWrite);
    QAbstractSocket::SocketState state() const;
    bool isConnected() const;
    void reconnect();

    QString getId() const;
    void setId(const QString &id);

protected:
    QTcpSocket *socket;
    QString identifier;

    const int DATA_STREAM_VERSION = QDataStream::Qt_5_4;
    int blockSize;
    QByteArray messageReceived;

signals:
    void connected();
    void received(QByteArray);
    void nameChanged();

public slots:
    void send(QByteArray message);

protected slots:
    void readMessage();
};

#endif // TCPSOCKET_H
