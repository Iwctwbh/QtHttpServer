//#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>
#include <QtSql>

#include "mysql.h"
#include "SimpleServers.h"

Mysql *mysql;

int main(int argc, char *argv[])
{
	[[maybe_unused]] const QCoreApplication *a = new QCoreApplication{argc, argv};

	SimpleServers simple_servers{};
	simple_servers.Run();
	qDebug() << "MSBuild Test Test";
	return QCoreApplication::exec();
}
