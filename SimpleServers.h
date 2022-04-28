#pragma once
#ifndef SIMPLESERVERS_H
#define SIMPLESERVERS_H

#include <QObject>
#include <QtNetwork>
class SimpleServers
{
public:
	typedef struct
	{
		//QByteArray *Control;
		QList<QByteArray> *Parameters;
		QByteArray *SQLString;

	} SimpleServer;
	void InitSimpleServers();
	void InsertSimpleServer(QByteArray *, QList<QByteArray> *, QByteArray *);
	void EraseSimpleServer();
	void InitSimpleServersFromJson(QSharedPointer<QJsonArray>);
	QMap<QByteArray *, SimpleServer *> *Map_SimpleServers = new QMap<QByteArray *, SimpleServer *>();
};

#endif // SIMPLESERVERS_H