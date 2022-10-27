#include "connection_pool_simple.h"

#include <QDebug>
#include <QtSql>
#include <QString>
#include <QThread>
#include <QCoreApplication>

// ��ȡ���ݿ�����
QSqlDatabase ConnectionPoolSimple::openConnection(const QString& connectionName)
{
	// 1. �������ӵ�ȫ��: �����̵߳ĵ�ַ�ʹ�������� connectionName����Ϊͬһ���߳̿������봴��������ݿ�����
	// 2. ��������Ѿ����ڣ������������������´���
	//    2.1 ��������ǰ�������ݿ⣬������ӶϿ����������½������� (����: �ر����ݿ⼸���Ӻ����������ٴη������ݿ�)
	// 3. ������Ӳ����ڣ��򴴽�����
	// 4. �߳̽���ʱ���ͷ��ڴ��߳��д��������ݿ�����

	// [1] �������ӵ�ȫ��: �����̵߳ĵ�ַ�ʹ�������� connectionName����Ϊͬһ���߳̿������봴��������ݿ�����
	QString baseConnectionName = "conn_" + QString::number(quint64(QThread::currentThread()), 16);
	QString fullConnectionName = baseConnectionName + connectionName;

	if (QSqlDatabase::contains(fullConnectionName))
	{
		// [2] ��������Ѿ����ڣ������������������´���
		QSqlDatabase existingDb = QSqlDatabase::database(fullConnectionName);

		// [2.1] ��������ǰ�������ݿ⣬������ӶϿ����������½������� (����: �ر����ݿ⼸���Ӻ����������ٴη������ݿ�)
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
	// [3] ������Ӳ����ڣ��򴴽�����
	if (qApp != nullptr)
	{
		// [4] �߳̽���ʱ���ͷ��ڴ��߳��д��������ݿ�����
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

// �������ݿ�����
QSqlDatabase ConnectionPoolSimple::createConnection(const QString& connectionName)
{
	static int sn = 0;

	// ����һ���µ����ݿ�����
	/*QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
	db.setHostName("localhost");
	db.setDatabaseName("qt");
	db.setUserName("root");
	db.setPassword("root");*/

	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", connectionName);
	db.setDatabaseName(QString("DRIVER={SQL SERVER};"
			"SERVER=%1;" //����������
			"DATABASE=%2;" //���ݿ���
			"UID=%3;" //��¼��
			"PWD=%4;" //����
			"SQL_ATTR_CONNECTION_POOLING=SQL_CP_ONE_PER_HENV;"
			"SQL_ATTR_ODBC_VERSION=SQL_OV_ODBC3;"
		).arg("172.28.99.74,1433") //Ĭ�ϵ�sqlserver�Ķ˿ں���1433
		 .arg("CAPS_DEV")
		 .arg("svc_portal_crm")
		 .arg("K97a1pBsvGk8xly6U") //��д���sa�˺ŵ����룡����������������������
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
