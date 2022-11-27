#include "connection_pool_simple.h"

#include <QDebug>
#include <QtSql>
#include <QString>
#include <QThread>
#include <QCoreApplication>

static QMap<QString, ConnectionPoolSimple::StructSqlServer> kMapSqlServers;

void ConnectionPoolSimple::init_sql_connect_by_json_object(const QJsonObject& arg_json_object_sql_servers)
{
	for (QJsonObject::const_iterator it = arg_json_object_sql_servers.constBegin(); it != arg_json_object_sql_servers.
	     constEnd(); ++it)
	{
		QJsonObject temp_json_object_sql = it.value().toObject();

		StructSqlServer temp_sql_server
		{
			it.key(),
			temp_json_object_sql.value("SQLDriver").toString(),
			temp_json_object_sql.value("Host").toString(),
			temp_json_object_sql.value("Port").toString(),
			temp_json_object_sql.value("UserName").toString(),
			temp_json_object_sql.value("Password").toString(),
			temp_json_object_sql.value("DataBase").toString(),
			temp_json_object_sql.value("Path").toString()
		};

		AddSqlServer(it.key(), temp_sql_server);
	}
}


void ConnectionPoolSimple::AddSqlServer(const QString& arg_key, const StructSqlServer& arg_sql_server)
{
	kMapSqlServers.insert(arg_key, arg_sql_server);
}

QMap<QString, ConnectionPoolSimple::StructSqlServer>& ConnectionPoolSimple::SqlServers()
{
	return kMapSqlServers;
}

// ��ȡ���ݿ�����
QSqlDatabase ConnectionPoolSimple::OpenConnection(StructSqlServer arg_struct_sql_server)
{
	// 1. �������ӵ�ȫ��: �����̵߳ĵ�ַ�ʹ�������� connectionName����Ϊͬһ���߳̿������봴��������ݿ�����
	// 2. ��������Ѿ����ڣ������������������´���
	//    2.1 ��������ǰ�������ݿ⣬������ӶϿ����������½������� (����: �ر����ݿ⼸���Ӻ����������ٴη������ݿ�)
	// 3. ������Ӳ����ڣ��򴴽�����
	// 4. �߳̽���ʱ���ͷ��ڴ��߳��д��������ݿ�����

	// [1] �������ӵ�ȫ��: �����̵߳ĵ�ַ�ʹ�������� connectionName����Ϊͬһ���߳̿������봴��������ݿ�����
	const QString base_connection_name = "conn_" + QString::number(reinterpret_cast<quint64>(QThread::currentThread()),
	                                                               16);
	QString full_connection_name = base_connection_name + "_" + arg_struct_sql_server.connection_name;

	if (QSqlDatabase::contains(full_connection_name))
	{
		// [2] ��������Ѿ����ڣ������������������´���
		QSqlDatabase existingDb = QSqlDatabase::database(full_connection_name);

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
		QObject::connect(QThread::currentThread(), &QThread::finished, qApp, [full_connection_name]
		{
			if (QSqlDatabase::contains(full_connection_name))
			{
				QSqlDatabase::removeDatabase(full_connection_name);
				qDebug().noquote() << QString("Connection deleted: %1").arg(full_connection_name);
			}
		});
	}
	arg_struct_sql_server.connection_name = full_connection_name;
	return CreateConnection(arg_struct_sql_server);
}

// �������ݿ�����
QSqlDatabase ConnectionPoolSimple::CreateConnection(const StructSqlServer& arg_struct_sql_server)
{
	static int sn = 0;

	if (const auto& [connection_name,sql_driver, host, port, user_name, password, data_base, path]{
			arg_struct_sql_server
		};
		!sql_driver.compare("MSSQL", Qt::CaseInsensitive))
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", connection_name);
		db.setDatabaseName(QString("DRIVER={SQL SERVER};"
				"SERVER=%1;" //����������
				"DATABASE=%2;" //���ݿ���
				"UID=%3;" //��¼��
				"PWD=%4;" //����
				"SQL_ATTR_CONNECTION_POOLING=SQL_CP_ONE_PER_HENV;"
				"SQL_ATTR_ODBC_VERSION=SQL_OV_ODBC3;"
			).arg(host + ',' + port) //Ĭ�ϵ�sqlserver�Ķ˿ں���1433
			 .arg(data_base)
			 .arg(user_name)
			 .arg(password) //��д���sa�˺ŵ����룡����������������������
		);
		db.setConnectOptions("SQL_ATTR_CONNECTION_POOLING=SQL_CP_ONE_PER_HENV;SQL_ATTR_ODBC_VERSION=SQL_OV_ODBC3;");

		if (db.open())
		{
			qDebug().noquote() << QString("Connection created: %1, sn: %2").arg(connection_name).arg(++sn);
			return db;
		}
		qDebug().noquote() << "Create connection error:" << db.lastError().text();
	}
	else if (!sql_driver.compare("MYSQL"))
	{
		// ����һ���µ����ݿ�����
		/*QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
		db.setHostName("localhost");
		db.setDatabaseName("qt");
		db.setUserName("root");
		db.setPassword("root");*/
	}
	else if (!sql_driver.compare("SQLITE"))
	{
		QSqlDatabase db{QSqlDatabase::addDatabase("QSQLITE", connection_name)};
		db.setDatabaseName(path);
		if (db.open())
		{
			qDebug().noquote() << QString("Connection created: %1, sn: %2").arg(connection_name).arg(++sn);
			return db;
		}
	}

	return QSqlDatabase();
}
