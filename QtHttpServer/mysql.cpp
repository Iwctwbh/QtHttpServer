#include "mysql.h"

#include <qsqlerror.h>
#include <quuid.h>

Mysql::Mysql()
{
}

Mysql::Mysql(QString SQLDriver, QString hostname, int port, QString dbname, QString username, QString pwd, QString connectionName)
{
	_hostname = hostname;
	_port = port;
	_dbname = dbname;
	_username = username;
	_pwd = pwd;
	_connectionName = connectionName;
	sql_driver_ = SQLDriver;
	_lastError = "no error";
}

bool Mysql::connect()
{
	bool result = true;
	/*db = QSqlDatabase::addDatabase(sql_driver_);
	db.setHostName(_hostname);
	db.setDatabaseName(_dbname);
	db.setUserName(_username);
	db.setPassword(_pwd);*/

	db = QSqlDatabase::addDatabase("QODBC", "1");
	db.addDatabase("QODBC", "2");
	db.addDatabase("QODBC", "4");
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
	/*db = QSqlDatabase::addDatabase("QODBC", "db1");
	db.setHostName("172.28.99.74");
	db.setPort(1433);
	db.setDatabaseName("CAPS_DEV");
	db.setUserName("svc_portal_crm");
	db.setPassword("K97a1pBsvGk8xly6U");*/
	//db.setConnectOptions("SQL_ATTR_CONNECTION_POOLING=SQL_CP_ONE_PER_DRIVER;");
	//db.setDatabaseName("db1");

	if (!db.open())
	{
		setLastError("MySQL Connect Error");
		result = false;
	}
	//this->close();

	return result;
}

void Mysql::close()
{
	/*if (db.isOpen())
	{
		db.close();
	}*/
}

QSqlQuery Mysql::QueryExec(QString sql)
{
	//static auto query = new QSqlQuery(QSqlDatabase::database(_connectionName));
	QSqlQuery query(db);
	query.setForwardOnly(true);
	if (!db.isOpen())
	{
		db.open();
	}
	if (!query.exec(sql))
	{
		qDebug() << query.lastError().text();
		setLastError("SQLString Query Error");
	}
	else
	{
		return query;
	}
	//this->close();
	return query;
}

Mysql::~Mysql()
{
	//this->close();
}

QString Mysql::lastError() const
{
	return _lastError;
}

void Mysql::setLastError(const QString& lastError)
{
	_lastError = lastError;
	qDebug() << "MySQL Error " << lastError;
}
