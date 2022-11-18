#pragma once
#ifndef SIMPLE_SERVERS_H
#define SIMPLE_SERVERS_H

#include <crow.h>
#include <QtCore/QtCore>
#include <QtSql/QtSql>

#include "common_tools.h"
#include "connection_pool_simple.h"
#include "log_helper_handler.h"

class SimpleServers final : public QObject
{
	Q_OBJECT
public:
	explicit SimpleServers(QObject* parent = nullptr);

	struct SimpleServer
	{
		// QByteArray *controller;
		QByteArray method{};
		QVector<QByteArray> vector_parameters{};
		QByteArray bytearray_sql{};
		QJsonObject json_object_data{};
		QJsonObject json_object_response{};
	};

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

		void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
		{
			// CROW_LOG_DEBUG << " - MESSAGE: " << string_message;
		}

		void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
		{
			// no-op
			// CROW_LOG_DEBUG << " - END";
		}
	};

	void InitSimpleServers(const QJsonObject& arg_json_object);
	void Run() const;

	void emit_run();

signals:
	void signal_run();

private:
	QJsonObject json_object_simple_server_{};

	QMap<QString, ConnectionPoolSimple::StructSqlServer> map_sql_servers_;
	QJsonObject json_object_sql_servers_;
};

class Controller
{
public:
	Controller(const QJsonObject& arg_json_object, QMap<QString, ConnectionPoolSimple::StructSqlServer> arg_map_sql_servers = {});
	QString GetValue(QString arg_string);

private:
	struct Sql
	{
		QString sql_name{};
		QString sql_query{};
		QHash<QString, QString> data{};
	};

	QSet<QString> set_methods_{};
	QHash<QString, QString> hash_parameters_{};
	QHash<QString, Sql> hash_sql_{};
	QHash<QString, QString> hash_data_;
	QHash<QString, QString> hash_response_{};
};

class Controllers
{
public:
	Controllers(const QJsonObject& arg_json_object);
private:
	QHash<QString, Controller> controllers_{};
};

#endif  // SIMPLE_SERVERS_H
