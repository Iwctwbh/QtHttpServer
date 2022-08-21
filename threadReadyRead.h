#pragma once
#ifndef THREADREADYREAD_H
#define THREADREADYREAD_H

#include <QObject>
#include <QtNetwork>
class threadReadyRead : public QObject
{
	Q_OBJECT
public:
	explicit threadReadyRead(QObject *parent = nullptr);
	void runLogicFunction();
	void setSocket(QTcpSocket *socket);
signals:
	void sendMessage(QByteArray v_byteArr);

private:
	QTcpSocket *t_socket;
};
#endif
