#pragma once
#ifndef MYSQL_H
#define MYSQL_H

#include <QSqlQuery>

class Mysql
{
public:
	Mysql();
	Mysql(QString SQLDriver, QString hostname, int port, QString dbname, QString username, QString pwd, QString connectionName);
	bool connect();
	void close();

	QSqlQuery QueryExec(QString sql);

	QString lastError() const;
	~Mysql(void);

private:
	QString _hostname;
	int _port;
	QString _dbname;
	QString _username;
	QString _pwd;
	QString _lastError;
	QString _lastSqlString;

	QSqlDatabase db;
	QString _connectionName;
	//QMutex mutex;
	QString sql_driver_; // QStringList drivers = QSqlDatabase::drivers();

	void setLastError(const QString& lastError);
};
#endif // MYSQL_H
