#include "connection_pool.h"
#include <QDebug>

// QMutex ConnectionPool::mutex;
// QWaitCondition ConnectionPool::waitConnection;
// ConnectionPool* ConnectionPool::instance = NULL;

QMutex ConnectionPool::g_thread_mutex;
std::map<int, ConnectionPool*> ConnectionPool::g_thread_instances;

ConnectionPool::ConnectionPool()
{
	// �������ݿ����ӵ���Щ��Ϣ��ʵ�ʿ�����ʱ����Ҫͨ����ȡ�����ļ��õ���
	// ����Ϊ����ʾ��������д�����˴����
	hostName = "127.0.0.1";
	databaseName = "qt";
	username = "root";
	password = "root";
	databaseType = "QMYSQL";
	testOnBorrow = true;
	testOnBorrowSql = "SELECT 1";

	maxWaitTime = 1000;
	waitInterval = 200;
	maxConnectionCount = 5;
}

ConnectionPool::~ConnectionPool()
{
	// �������ӳص�ʱ��ɾ�����е�����
	foreach(QString connectionName, usedConnectionNames)
	{
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames)
	{
		QSqlDatabase::removeDatabase(connectionName);
	}
}

ConnectionPool& ConnectionPool::getInstance()
{
	auto _tid = QThread::currentThreadId();
	ConnectionPool* instance = nullptr;
	int _itid = reinterpret_cast<int>(_tid);

	QMutexLocker locker(&g_thread_mutex);
	auto itr = g_thread_instances.find(_itid);
	if (g_thread_instances.empty() || g_thread_instances.end() == itr)
	{
		instance = new ConnectionPool();
		g_thread_instances[_itid] = instance;
	}
	else
	{
		instance = itr->second;
	}

	//if (NULL == instance) {
	//	QMutexLocker locker(&mutex);

	//	if (NULL == instance) {
	//		instance = new ConnectionPool();
	//	}
	//}

	return *instance;
}

void ConnectionPool::release()
{
	QMutexLocker locker(&g_thread_mutex);

	//�Ƴ��������ݿ�����
	QStringList list = QSqlDatabase::connectionNames();
	QList<QString>::Iterator it = list.begin(), itend = list.end();
	for (; it != itend; ++it)
	{
		QSqlDatabase::removeDatabase(*it);
	}

	auto itr = g_thread_instances.begin();
	for (; itr != g_thread_instances.end(); ++itr)
	{
		auto _tmp = itr->second;
		itr->second = nullptr;
		delete _tmp;
		_tmp = nullptr;
	}

	g_thread_instances.clear();

	// QMutexLocker locker(&mutex);
	// delete instance;
	// instance = NULL;
}

QSqlDatabase ConnectionPool::openConnection()
{
	ConnectionPool& pool = getInstance();
	QString connectionName;

	QMutexLocker locker(&mutex);

	// �Ѵ���������
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

	// ��������Ѿ����꣬�ȴ� waitInterval ���뿴���Ƿ��п������ӣ���ȴ� maxWaitTime ����
	for (int i = 0;
	     i < pool.maxWaitTime
	     && pool.unusedConnectionNames.size() == 0 && connectionCount == pool.maxConnectionCount;
	     i += pool.waitInterval)
	{
		waitConnection.wait(&mutex, pool.waitInterval);

		// ���¼����Ѵ���������
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}

	if (pool.unusedConnectionNames.size() > 0)
	{
		// ���Ѿ����յ����ӣ���������
		connectionName = pool.unusedConnectionNames.dequeue();
	}
	else if (connectionCount < pool.maxConnectionCount)
	{
		// û���Ѿ����յ����ӣ�����û�дﵽ������������򴴽��µ�����
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else
	{
		// �Ѿ��ﵽ���������
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}

	// ��������
	QSqlDatabase db = pool.createConnection(connectionName);

	// ��Ч�����Ӳŷ��� usedConnectionNames
	if (db.isOpen())
	{
		pool.usedConnectionNames.enqueue(connectionName);
	}

	return db;
}

void ConnectionPool::closeConnection(QSqlDatabase connection)
{
	ConnectionPool& pool = getInstance();
	QString connectionName = connection.connectionName();

	// ��������Ǵ��������ӣ��� used ��ɾ�������� unused ��
	if (pool.usedConnectionNames.contains(connectionName))
	{
		QMutexLocker locker(&mutex);
		pool.usedConnectionNames.removeOne(connectionName);
		pool.unusedConnectionNames.enqueue(connectionName);
		waitConnection.wakeOne();
	}
}

QSqlDatabase ConnectionPool::createConnection(const QString& connectionName) const
{
	// �����Ѿ��������ˣ������������������´���
	if (QSqlDatabase::contains(connectionName))
	{
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);

		if (testOnBorrow)
		{
			// ��������ǰ�������ݿ⣬������ӶϿ������½�������
			qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
			QSqlQuery query(testOnBorrowSql, db1);

			if (query.lastError().type() != QSqlError::NoError && !db1.open())
			{
				qDebug() << "Open datatabase error:" << db1.lastError().text();
				return QSqlDatabase();
			}
		}

		return db1;
	}

	// ����һ���µ�����
	QSqlDatabase db = QSqlDatabase::addDatabase(databaseType, connectionName);
	db.setHostName(hostName);
	db.setDatabaseName(databaseName);
	db.setUserName(username);
	db.setPassword(password);

	if (!db.open())
	{
		qDebug() << "Open datatabase error:" << db.lastError().text();
		return QSqlDatabase();
	}

	return db;
}

void ConnectionPool::releaseThreadPool()
{
	auto _tid = QThread::currentThreadId();
	ConnectionPool* instance = nullptr;
	int _itid = reinterpret_cast<int>(_tid);

	QMutexLocker locker(&g_thread_mutex);
	auto itr = g_thread_instances.find(_itid);
	if (itr != g_thread_instances.end())
	{
		auto _tmp = itr->second;
		g_thread_instances.erase(itr);
		delete _tmp;
		_tmp = nullptr;
	}
}
