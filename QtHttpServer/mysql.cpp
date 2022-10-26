#include "mysql.h"

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

	db = QSqlDatabase::addDatabase("QODBC", QUuid::createUuidV5(QUuid::createUuid(), QUuid::createUuid().toString()).toString());
	db.setDatabaseName(QString("DRIVER={SQL SERVER};"
			"SERVER=%1;" //����������
			"DATABASE=%2;" //���ݿ���
			"UID=%3;" //��¼��
			"PWD=%4;" //����
		).arg("172.28.99.74,1433") //Ĭ�ϵ�sqlserver�Ķ˿ں���1433
		 .arg("CAPS_DEV")
		 .arg("svc_portal_crm")
		 .arg("K97a1pBsvGk8xly6U") //��д���sa�˺ŵ����룡����������������������
	);

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
	if (db.isOpen())
	{
		db.close();
	}
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
