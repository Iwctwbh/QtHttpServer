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

	QMap<QString, ConnectionPoolSimple::StructSqlServer> map_sql_servers_{};
	QJsonObject json_object_sql_servers_{};
	quint16 int_port_{};
	quint16 int_log_level_{};
	QByteArray string_ip_address_{};
	QJsonObject json_object_controllers_{};
};

class Controller
{
public:
	Controller();
	explicit Controller(const QJsonObject& arg_json_object);
	bool IsInMethods(const QString& arg_method) const;
	bool IsResponseEmpty() const;
	void MappingData();
	QHash<QString, QString> GetResponse();
	QString GetValue(QString arg_value, QString arg_key);

private:
	struct Sql
	{
		QString sql_name{};
		QString sql_query{};
		QString sql_result{};
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
	explicit Controllers(const QJsonObject& arg_json_object);

	/**
	 * \brief 获取对应控制器
	 * \param arg_name 控制器名称
	 * \return 控制器类
	 */
	Controller GetController(const QString& arg_name) const;
private:
	QHash<QString, Controller> controllers_{};
};

#endif  // SIMPLE_SERVERS_H
