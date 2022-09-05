﻿//#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>

#include "mysql.h"
#include "SimpleServers.h"
#include "qt_common_tools.h"

#pragma execution_character_set("utf-8")

//#pragma comment(lib, "QtCommonTools.lib")

Mysql *mysql;

int main(int argc, char *argv[])
{
	[[maybe_unused]] const QCoreApplication *a = new QCoreApplication{argc, argv};
	auto b = QtCommonTools::ConvertImgToBase64("../Resource/YQRC.png");
	SimpleServers simple_servers{};
	simple_servers.Run();
	qDebug() << "MSBuild Test Test";
	return QCoreApplication::exec();
}
