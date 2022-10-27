#include "thread_connect_sql.h"
#include "connection_pool.h"
#include <QDebug>

ThreadConnectSql::ThreadConnectSql(QObject* parent) : QThread(parent)
{
}

static void foo()
{
	// [1] 从数据库连接池里取得连接
	QSqlDatabase db = ConnectionPool::openConnection();

	// [2] 使用连接查询数据库
	QSqlQuery query(db);
	query.exec("SELECT TOP 1 * FROM tblConfig");

	while (query.next())
	{
		qDebug() << query.value("username").toString();
	}
}

void ThreadConnectSql::run()
{
	foo();
	sleep(1);
	foo();
}
