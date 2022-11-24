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
	 * @brief ��ȡ���ݿ����ӣ�����ʹ�������Ҫ�ֶ��رգ����ݿ����ӳػ���ʹ�ô����ӵ��߳̽������Զ��ر����ӡ�
	 * ����������� connectionName Ĭ��Ϊ�� (�ڲ���Ϊ�����������̵߳���Ϣ����һ��Ψһ��ǰ׺)��
	 * ���ͬһ���߳���Ҫʹ�ö����ͬ�����ݿ����ӣ����Դ��벻ͬ�� connectionName
	 *
	 * @param arg_struct_sql_server ���ӵ�����
	 * @return �������ݿ�����
	 */
	static QSqlDatabase OpenConnection(StructSqlServer arg_struct_sql_server);

	/**
	 * \brief ����json�����Զ���ʼ��
	 * \param arg_json_object_sql_servers json ����
	 */
	static void init_sql_connect_by_json_object(const QJsonObject& arg_json_object_sql_servers);

	/**
	 * \brief ���SqlServer
	 * \param arg_key key
	 * \param arg_sql_server value
	 */
	static void AddSqlServer(const QString& arg_key, const StructSqlServer& arg_sql_server);

	/**
	 * \brief ��ȡ����SqlServers
	 * \return ����SqlServers
	 */
	static QMap<QString, StructSqlServer>& SqlServers();

private:
	/**
	 * \brief �������ݿ�����
	 * \param arg_struct_sql_server ���Ӷ���
	 * \return QSqlDatabase
	 */
	static QSqlDatabase CreateConnection(const StructSqlServer& arg_struct_sql_server);
};

#endif // CONNECTION_POOL_H
