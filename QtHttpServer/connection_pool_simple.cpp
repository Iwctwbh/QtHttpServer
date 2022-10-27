#include "connection_pool_simple.h"

#include <QDebug>
#include <QtSql>
#include <QString>
#include <QThread>
#include <QCoreApplication>

// 获取数据库连接
QSqlDatabase ConnectionPoolSimple::openConnection(const QString& connectionName)
{
	// 1. 创建连接的全名: 基于线程的地址和传入进来的 connectionName，因为同一个线程可能申请创建多个数据库连接
	// 2. 如果连接已经存在，复用它，而不是重新创建
	//    2.1 返回连接前访问数据库，如果连接断开，可以重新建立连接 (测试: 关闭数据库几分钟后再启动，再次访问数据库)
	// 3. 如果连接不存在，则创建连接
	// 4. 线程结束时，释放在此线程中创建的数据库连接

	// [1] 创建连接的全名: 基于线程的地址和传入进来的 connectionName，因为同一个线程可能申请创建多个数据库连接
	QString baseConnectionName = "conn_" + QString::number(quint64(QThread::currentThread()), 16);
	QString fullConnectionName = baseConnectionName + connectionName;

	if (QSqlDatabase::contains(fullConnectionName))
	{
		// [2] 如果连接已经存在，复用它，而不是重新创建
		QSqlDatabase existingDb = QSqlDatabase::database(fullConnectionName);

		// [2.1] 返回连接前访问数据库，如果连接断开，可以重新建立连接 (测试: 关闭数据库几分钟后再启动，再次访问数据库)
		/*QSqlQuery query("SELECT 1", existingDb);

		if (query.lastError().type() != QSqlError::NoError && !existingDb.open())
		{
			qDebug().noquote() << "Open datatabase error:" << existingDb.lastError().text();
			return QSqlDatabase();
		}*/

		if (!existingDb.isOpen())
		{
			existingDb.open();
		}

		return existingDb;
	}
	// [3] 如果连接不存在，则创建连接
	if (qApp != nullptr)
	{
		// [4] 线程结束时，释放在此线程中创建的数据库连接
		QObject::connect(QThread::currentThread(), &QThread::finished, qApp, [fullConnectionName]
		{
			if (QSqlDatabase::contains(fullConnectionName))
			{
				QSqlDatabase::removeDatabase(fullConnectionName);
				qDebug().noquote() << QString("Connection deleted: %1").arg(fullConnectionName);
			}
		});
	}

	return createConnection(fullConnectionName);
}

// 创建数据库连接
QSqlDatabase ConnectionPoolSimple::createConnection(const QString& connectionName)
{
	static int sn = 0;

	// 创建一个新的数据库连接
	/*QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
	db.setHostName("localhost");
	db.setDatabaseName("qt");
	db.setUserName("root");
	db.setPassword("root");*/

	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", connectionName);
	db.setDatabaseName(QString("DRIVER={SQL SERVER};"
			"SERVER=%1;" //服务器名称
			"DATABASE=%2;" //数据库名
			"UID=%3;" //登录名
			"PWD=%4;" //密码
			"SQL_ATTR_CONNECTION_POOLING=SQL_CP_ONE_PER_HENV;"
			"SQL_ATTR_ODBC_VERSION=SQL_OV_ODBC3;"
		).arg("172.28.99.74,1433") //默认的sqlserver的端口号是1433
		 .arg("CAPS_DEV")
		 .arg("svc_portal_crm")
		 .arg("K97a1pBsvGk8xly6U") //填写你的sa账号的密码！！！！！！！！！！！！
	);
	db.setConnectOptions("SQL_ATTR_CONNECTION_POOLING=SQL_CP_ONE_PER_HENV;SQL_ATTR_ODBC_VERSION=SQL_OV_ODBC3;");

	if (db.open())
	{
		qDebug().noquote() << QString("Connection created: %1, sn: %2").arg(connectionName).arg(++sn);
		return db;
	}
	qDebug().noquote() << "Create connection error:" << db.lastError().text();
	return QSqlDatabase();
}
