#pragma once
#ifndef SIMPLESERVERS_H
#define SIMPLESERVERS_H

#include <QtNetwork>
class SimpleServers
{
public:
	SimpleServers();
	typedef struct
	{
		//QByteArray *controller;
		QList<QByteArray> list_parameters;
		QByteArray bytearray_sql;

	} SimpleServer;
	auto InitSimpleServers() -> void;
	auto InsertSimpleServer(const QByteArray&, const QList<QByteArray>&, const QByteArray&) -> void;
	static auto EraseSimpleServer() -> void;
	auto InitSimpleServersFromJson(const QJsonArray&) -> void;
	QMap<QByteArray, SimpleServer> map_simple_servers_{};
};

#endif // SIMPLESERVERS_H
