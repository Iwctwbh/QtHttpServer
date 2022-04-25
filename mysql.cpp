#include "mysql.h"

Mysql::Mysql(QString hostname, int port, QString dbname, QString username, QString pwd, QString connectionName)
{
	_hostname = hostname;
	_port = port;
	_dbname = dbname;
	_username = username;
	_pwd = pwd;
	_connectionName = connectionName;
	_lastError = "no error";
}

bool Mysql::connect()
{
	bool result = true;
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(_hostname);
	db.setDatabaseName(_dbname);
	db.setUserName(_username);
	db.setPassword(_pwd);
	if (!db.open())
	{
		setLastError("���ݿ�����ʧ�� ");
		result = false;
	}

	return result;
}

void Mysql::close()
{
	if (db.isOpen())
	{
		db.close();
	}
}

QSqlQuery* Mysql::QueryExec(QString sql)
{
	static QSqlQuery *query = new QSqlQuery(QSqlDatabase::database(_connectionName));
	if (db.isOpen())
	{
		if (!query->exec(sql))
		{
			setLastError("ִ��ʧ�� ");
		}
		else
		{
			return query;
		}
	}
	else
	{
		setLastError("���ݿ�δ�� ");
	}
	return query;
}

Mysql::~Mysql()
{
	this->close();
}

QString Mysql::lastError() const
{
	return _lastError;
}

void Mysql::setLastError(const QString &lastError)
{
	_lastError = lastError;
}