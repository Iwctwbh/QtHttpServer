#pragma once
#ifndef SIMPLE_SERVERS_H
#define SIMPLE_SERVERS_H

#include <crow.h>
#include <QtCore/QtCore>

#include "log_helper_handler.h"
//#include "qt_common_tools.h"
#include "common_tools.h"

class SimpleServers final : public QObject
{
	Q_OBJECT
public:
	explicit SimpleServers(QObject *parent = nullptr);

	struct SimpleServer
	{
		//QByteArray *controller;
		QByteArray method{};
		QList<QByteArray> list_parameters{};
		QByteArray bytearray_sql{};
		QJsonObject json_object_data{};
		QJsonObject json_object_response{};
	};

	static std::vector<QByteArray> GetVectorMethodStrings()
	{
		std::vector<QByteArray> list_method_strings{};
		std::ranges::for_each(crow::method_strings, [&](const char *temp_ptr_char)
		{
			list_method_strings.emplace_back(temp_ptr_char);
		});
		return list_method_strings;
	}

	struct SimpleServerMiddleware : crow::ILocalMiddleware
	{
		QString string_message{};

		void SetMessage(const QString &arg_message)
		{
			string_message = arg_message;
		}

		struct context
		{
		};

		void before_handle(crow::request & /*req*/, crow::response & /*res*/, context & /*ctx*/) const
		{
			//CROW_LOG_DEBUG << " - MESSAGE: " << string_message;
		}

		void after_handle(crow::request & /*req*/, crow::response & /*res*/, context & /*ctx*/)
		{
			// no-op
			//CROW_LOG_DEBUG << " - END";
		}
	};

	void InitSimpleServers();
	void InsertSimpleServer(const QByteArray &arg_bytearray_controller,
							const QByteArray &arg_bytearray_method,
							const QList<QByteArray> &arg_list_parameters,
							const QByteArray &arg_bytearray_sql,
							const QJsonObject &arg_json_object_data,
							const QJsonObject &arg_bytearray_response);
	static void EraseSimpleServer();
	void InitSimpleServersFromJson(const QJsonArray &arg_json_array);
	QHash<QByteArray, SimpleServer> &GetSimpleServersMap();
	void Run();

	void emit_run();

signals:
	void signal_run();

private:
	QHash<QByteArray, SimpleServer> map_simple_servers_{};
	LogHelperHandler handler_log_helper_{};
	QByteArray bytearray_http_server_ip_address_{};
	uint16_t uint_http_server_port_{};
	uint16_t unit_http_server_log_level_{};
};

#endif // SIMPLE_SERVERS_H