#pragma once
#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class ConnectionPool
{
public:
	ConnectionPool();
	static void release(); // �ر����е����ݿ�����
	QSqlDatabase openConnection(); // ��ȡ���ݿ�����
	void closeConnection(QSqlDatabase connection); // �ͷ����ݿ����ӻ����ӳ�

	static void releaseThreadPool(); // �ͷŴ��߳��µ�ȫ�����ӳ�

	~ConnectionPool();

private:
	static ConnectionPool& getInstance();

	//ConnectionPool();
	ConnectionPool(const ConnectionPool& other);
	ConnectionPool& operator=(const ConnectionPool& other);
	QSqlDatabase createConnection(const QString& connectionName); // �������ݿ�����

	QQueue<QString> usedConnectionNames; // ��ʹ�õ����ݿ�������
	QQueue<QString> unusedConnectionNames; // δʹ�õ����ݿ�������

	// ���ݿ���Ϣ
	QString hostName;
	QString databaseName;
	QString username;
	QString password;
	QString databaseType;

	bool testOnBorrow; // ȡ�����ӵ�ʱ����֤�����Ƿ���Ч
	QString testOnBorrowSql; // ���Է������ݿ�� SQL

	int maxWaitTime; // ��ȡ�������ȴ�ʱ��
	int waitInterval; // ���Ի�ȡ����ʱ�ȴ����ʱ��
	int maxConnectionCount; // ���������

	// static QMutex mutex;
	// static QWaitCondition waitConnection;
	// static ConnectionPool *instance;

	QMutex mutex;
	QWaitCondition waitConnection;

	static QMutex g_thread_mutex;
	static std::map<int, ConnectionPool*> g_thread_instances;
};

#endif // CONNECTION_POOL_H
