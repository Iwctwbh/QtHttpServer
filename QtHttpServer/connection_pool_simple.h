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
	};

	/**
	 * @brief ��ȡ���ݿ����ӣ�����ʹ�������Ҫ�ֶ��رգ����ݿ����ӳػ���ʹ�ô����ӵ��߳̽������Զ��ر����ӡ�
	 * ����������� connectionName Ĭ��Ϊ�� (�ڲ���Ϊ�����������̵߳���Ϣ����һ��Ψһ��ǰ׺)��
	 * ���ͬһ���߳���Ҫʹ�ö����ͬ�����ݿ����ӣ����Դ��벻ͬ�� connectionName
	 *
	 * @param connectionName ���ӵ�����
	 * @return �������ݿ�����
	 */
	static QSqlDatabase openConnection(StructSqlServer arg_struct_sql_server);

private:
	static QSqlDatabase createConnection(const StructSqlServer& arg_struct_sql_server); // �������ݿ�����
};

#endif // CONNECTION_POOL_H
