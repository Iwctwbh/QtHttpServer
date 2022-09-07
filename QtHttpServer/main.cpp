//#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>

#include "mysql.h"
#include "simple_servers.h"

#pragma execution_character_set("utf-8")

//#pragma comment(lib, "QtCommonTools.lib")

Mysql *mysql;

int main(int argc, char *argv[])
{
	[[maybe_unused]] const QCoreApplication *a = new QCoreApplication{ argc, argv };

	// QThread
	QThread * const thread_simple_servers = new QThread();
	SimpleServers * const simple_servers = new SimpleServers();
	simple_servers->moveToThread(thread_simple_servers);
	thread_simple_servers->start();
	QObject::connect(simple_servers, &SimpleServers::signal_run, simple_servers, &SimpleServers::Run);
	simple_servers->emit_run();

	//auto b = QtCommonTools::ConvertImgToBase64("../Resource/YQRC.png");

	qDebug() << "...Success";
	return QCoreApplication::exec();
}
