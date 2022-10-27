#pragma once
#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

class ConnectionPool
{
public:
	/**
	 * @brief ��ȡ���ݿ����ӣ�����ʹ�������Ҫ�ֶ��رգ����ݿ����ӳػ���ʹ�ô����ӵ��߳̽������Զ��ر����ӡ�
	 * ����������� connectionName Ĭ��Ϊ�� (�ڲ���Ϊ�����������̵߳���Ϣ����һ��Ψһ��ǰ׺)��
	 * ���ͬһ���߳���Ҫʹ�ö����ͬ�����ݿ����ӣ����Դ��벻ͬ�� connectionName
	 *
	 * @param connectionName ���ӵ�����
	 * @return �������ݿ�����
	 */
	static QSqlDatabase openConnection(const QString& connectionName = QString());

private:
	static QSqlDatabase createConnection(const QString& connectionName); // �������ݿ�����
};

#endif // CONNECTION_POOL_H
