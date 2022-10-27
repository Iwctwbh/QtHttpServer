#pragma once
#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

class ConnectionPool
{
public:
	/**
	 * @brief 获取数据库连接，连接使用完后不需要手动关闭，数据库连接池会在使用此连接的线程结束后自动关闭连接。
	 * 传入的连接名 connectionName 默认为空 (内部会为连接名基于线程的信息创建一个唯一的前缀)，
	 * 如果同一个线程需要使用多个不同的数据库连接，可以传入不同的 connectionName
	 *
	 * @param connectionName 连接的名字
	 * @return 返回数据库连接
	 */
	static QSqlDatabase openConnection(const QString& connectionName = QString());

private:
	static QSqlDatabase createConnection(const QString& connectionName); // 创建数据库连接
};

#endif // CONNECTION_POOL_H
