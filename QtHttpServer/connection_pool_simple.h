#pragma once
#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <qjsonobject.h>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

class ConnectionPoolSimple
{
public:
	struct StructSqlServer
	{
		QString connection_name;
		QString sql_driver;
		QString host;
		QString port;
		QString user_name;
		QString password;
		QString data_base;
		QString path;
	};

	/**
	 * @brief 获取数据库连接，连接使用完后不需要手动关闭，数据库连接池会在使用此连接的线程结束后自动关闭连接。
	 * 传入的连接名 connectionName 默认为空 (内部会为连接名基于线程的信息创建一个唯一的前缀)，
	 * 如果同一个线程需要使用多个不同的数据库连接，可以传入不同的 connectionName
	 *
	 * @param arg_struct_sql_server 连接的名字
	 * @return 返回数据库连接
	 */
	static QSqlDatabase OpenConnection(StructSqlServer arg_struct_sql_server);

	/**
	 * \brief 根据json对象自动初始化
	 * \param arg_json_object_sql_servers json 对象
	 */
	static void init_sql_connect_by_json_object(const QJsonObject& arg_json_object_sql_servers);

	/**
	 * \brief 添加SqlServer
	 * \param arg_key key
	 * \param arg_sql_server value
	 */
	static void AddSqlServer(const QString& arg_key, const StructSqlServer& arg_sql_server);

	/**
	 * \brief 获取所有SqlServers
	 * \return 所有SqlServers
	 */
	static QMap<QString, StructSqlServer>& SqlServers();

private:
	/**
	 * \brief 创建数据库连接
	 * \param arg_struct_sql_server 连接对象
	 * \return QSqlDatabase
	 */
	static QSqlDatabase CreateConnection(const StructSqlServer& arg_struct_sql_server);
};

#endif // CONNECTION_POOL_H
