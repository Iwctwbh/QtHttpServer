#pragma once
#ifndef SIMPLESERVERS_H
#define SIMPLESERVERS_H

#include <QtNetwork>
class SimpleServers
{
public:
	SimpleServers();
	struct SimpleServer
	{
		//QByteArray *controller;
		QString method;
		QList<QByteArray> list_parameters;
		QByteArray bytearray_sql;

	};
	auto InitSimpleServers() -> void;
	auto InsertSimpleServer(const QByteArray&, const QByteArray&, const QList<QByteArray>&, const QByteArray&) -> void;
	static auto EraseSimpleServer() -> void;
	auto InitSimpleServersFromJson(const QJsonArray&) -> void;
	auto GetSimpleServersMap(void)->QMap<QByteArray, SimpleServer>;

private:
	QMap<QByteArray, SimpleServer> map_simple_servers_{};
};

#endif // SIMPLESERVERS_H
