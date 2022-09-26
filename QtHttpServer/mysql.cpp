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
		setLastError("MySQL Connect Error");
		result = false;
	}
	this->close();

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
	static auto query = new QSqlQuery(QSqlDatabase::database(_connectionName));
	if (db.open())
	{
		if (!query->exec(sql))
		{
			setLastError("SQLString Query Error");
		}
		else
		{
			return query;
		}
	}
	else
	{
		setLastError("MySQL Connection Closed");
	}
	this->close();
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

void Mysql::setLastError(const QString& lastError)
{
	_lastError = lastError;
	qDebug() << "MySQL Error " << lastError;
}
