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

	if (QFile file_simple_server{ "SimpleServer.json" }; file_simple_server.exists())
	{
		qDebug() << "SimpleServer.json exists";
		if (file_simple_server.open(QIODevice::ReadOnly))
		{
			qDebug() << "SimpleServer.json open success";
			const QByteArray bytearray_simple_server{ file_simple_server.readAll() };
			QJsonParseError error_json_parse{ QJsonParseError::NoError };
			if (const QJsonDocument json_document_simple_server_file{ QJsonDocument::fromJson(bytearray_simple_server, &error_json_parse) }; error_json_parse.error == QJsonParseError::NoError && json_document_simple_server_file.isObject())
			{
				QJsonObject json_object_simple_servers{ json_document_simple_server_file.object().value("SimpleServers").toObject() };
				std::ranges::for_each(json_object_simple_servers.keys(), [&json_object_simple_servers](const QJsonValue &temp_json_value)
				{
					// QThread
					QThread *const thread_simple_servers = new QThread();
					SimpleServers *const simple_servers = new SimpleServers();

					simple_servers->InitSimpleServers(json_object_simple_servers.value(temp_json_value.toString()).toObject());
					simple_servers->moveToThread(thread_simple_servers);
					thread_simple_servers->start();
					QObject::connect(simple_servers, &SimpleServers::signal_run, simple_servers, &SimpleServers::Run);
					simple_servers->emit_run();
				});
			}
			else
			{
				qDebug() << "SimpleServer.json format is not correct";
			}
		}
	}
	qDebug() << "...Success";
	QtCommonTools abc{};
	qDebug() << abc.GetMiCode();
	return QCoreApplication::exec();
}
