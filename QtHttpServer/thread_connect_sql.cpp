#include "thread_connect_sql.h"
#include "connection_pool.h"
#include <QDebug>

ThreadConnectSql::ThreadConnectSql(QObject* parent) : QThread(parent)
{
}

static void foo()
{
	// [1] �����ݿ����ӳ���ȡ������
	QSqlDatabase db = ConnectionPool::openConnection();

	// [2] ʹ�����Ӳ�ѯ���ݿ�
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
