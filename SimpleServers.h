#pragma once
#ifndef SIMPLE_SERVERS_H
#define SIMPLE_SERVERS_H

#include <QtNetwork>
#include <crow.h>

#include "LogHelperHandler.h"

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

	static constexpr std::vector<QString> GetVectorMethodStrings()
	{
		std::vector<QString> list_method_strings{};
		std::ranges::for_each(crow::method_strings, [&](const char* temp_ptr_char)
		{
			list_method_strings.emplace_back(temp_ptr_char);
		});
		return list_method_strings;
	}

	struct SimpleServerMiddleware : crow::ILocalMiddleware
	{
		QString string_message{};

		void SetMessage(const QString& arg_message)
		{
			string_message = arg_message;
		}

		struct context
		{
		};

		void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) const
		{
			//CROW_LOG_DEBUG << " - MESSAGE: " << string_message;
		}

		void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
		{
			// no-op
			//CROW_LOG_DEBUG << " - END";
		}
	};

	void InitSimpleServers();
	void InsertSimpleServer(const QByteArray&, const QByteArray&, const QList<QByteArray>&, const QByteArray&);
	static void EraseSimpleServer();
	void InitSimpleServersFromJson(const QJsonArray&);
	QMap<QByteArray, SimpleServer> GetSimpleServersMap();
	void Run();

private:
	QMap<QByteArray, SimpleServer> map_simple_servers_{};
	LogHelperHandler handler_log_helper_;
};

#endif // SIMPLE_SERVERS_H
