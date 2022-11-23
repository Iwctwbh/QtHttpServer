#include "connection_pool.h"
#include <QDebug>

// QMutex ConnectionPool::mutex;
// QWaitCondition ConnectionPool::waitConnection;
// ConnectionPool* ConnectionPool::instance = NULL;

QMutex ConnectionPool::g_thread_mutex;
std::map<int, ConnectionPool*> ConnectionPool::g_thread_instances;

ConnectionPool::ConnectionPool()
{
	// 创建数据库连接的这些信息在实际开发的时都需要通过读取配置文件得到，
	// 这里为了演示方便所以写死在了代码里。
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
	// 销毁连接池的时候删除所有的连接
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

	//移除所有数据库连接
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

	// 已创建连接数
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

	// 如果连接已经用完，等待 waitInterval 毫秒看看是否有可用连接，最长等待 maxWaitTime 毫秒
	for (int i = 0;
	     i < pool.maxWaitTime
	     && pool.unusedConnectionNames.size() == 0 && connectionCount == pool.maxConnectionCount;
	     i += pool.waitInterval)
	{
		waitConnection.wait(&mutex, pool.waitInterval);

		// 重新计算已创建连接数
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}

	if (pool.unusedConnectionNames.size() > 0)
	{
		// 有已经回收的连接，复用它们
		connectionName = pool.unusedConnectionNames.dequeue();
	}
	else if (connectionCount < pool.maxConnectionCount)
	{
		// 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else
	{
		// 已经达到最大连接数
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}

	// 创建连接
	QSqlDatabase db = pool.createConnection(connectionName);

	// 有效的连接才放入 usedConnectionNames
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

	// 如果是我们创建的连接，从 used 里删除，放入 unused 里
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
	// 连接已经创建过了，复用它，而不是重新创建
	if (QSqlDatabase::contains(connectionName))
	{
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);

		if (testOnBorrow)
		{
			// 返回连接前访问数据库，如果连接断开，重新建立连接
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

	// 创建一个新的连接
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
