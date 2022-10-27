#pragma once
#ifndef THREAD_CONNECT_SQL_H
#define THREAD_CONNECT_SQL_H

#include <QThread>

class ThreadConnectSql : public QThread
{
	Q_OBJECT
public:
	explicit ThreadConnectSql(QObject* parent = nullptr);

protected:
	void run() override;
};

#endif // THREAD_CONNECT_SQL_H
