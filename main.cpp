//#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>
#include <QtSql>

#include "mysql.h"
#include "RequestParse.h"
#include "SimpleServers.h"
#include "drogon/drogon.h"
#include "crow.h"

void Logiccontroller(QByteArray* byteArr_Request, QByteArray* byteArr_ResponseHttp, QByteArray* byteArr_ResponseData);

Mysql* mysql;

int main(int argc, char* argv[])
{
	const QCoreApplication* a = new QCoreApplication{ argc, argv };
	if (QFile file_simple_server{ "SimpleServer.json" }; file_simple_server.exists())
	{
		qDebug() << "SimpleServer.json exists";
		qDebug() << "SimpleServer.json is readable";
		if (file_simple_server.open(QIODevice::ReadOnly))
		{
			qDebug() << "SimpleServer.json open success";
			const QByteArray bytearray_simple_server = file_simple_server.readAll();
			QJsonParseError error_json_parse{ QJsonParseError::NoError };
			if (const QJsonDocument json_document_simple_server_file{ QJsonDocument::fromJson(bytearray_simple_server, &error_json_parse) }; error_json_parse.error == QJsonParseError::NoError && json_document_simple_server_file.isObject())
			{
				qDebug() << "SimpleServer.json format is correct";
				const QJsonObject json_object_simple_server{ json_document_simple_server_file.object() };
				const QJsonObject json_object_mysql{ json_object_simple_server.value("MySQL").toObject() };
				mysql = new Mysql(json_object_mysql.value("Host").toString(),
								  json_object_mysql.value("Port").toString().toInt(),
								  json_object_mysql.value("DataBase").toString(),
								  json_object_mysql.value("UserName").toString(),
								  json_object_mysql.value("Password").toString(),
								  "db_connect");

				if (const QJsonDocument json_document_simple_server{ QJsonDocument{json_object_simple_server.value("SimpleServers").toArray()} }; json_document_simple_server.array().count() > 0)
				{
					qDebug() << "SimpleServers JsonArray is correct";
					const QJsonArray json_array_simple_server{ QJsonArray{json_document_simple_server.array()} };
					const quint16 int_http_server_port = json_object_simple_server.value("HttpServerPort").toString().toUInt();
					const QString string_http_server_ip_address = json_object_simple_server.value("HttpServerIPAddress").toString();

					// Init SimpleServers
					SimpleServers simple_servers{};
					simple_servers.InitSimpleServersFromJson(json_array_simple_server);

					// 连接数据库
					mysql->connect();

					// Init Crow
					crow::SimpleApp simple_app_crow{};
					std::ranges::for_each(simple_servers.GetSimpleServersMap().keys(), [&simple_app_crow, &simple_servers](const QByteArray& temp_bytearray_key)
					{
						simple_app_crow.route_dynamic(std::string(temp_bytearray_key))([](const crow::request& request_request)
						{
							auto a = method_name(request_request.method);
							return a;
						});
					});
					simple_app_crow.loglevel(crow::LogLevel::Info);
					simple_app_crow.bindaddr(std::string(string_http_server_ip_address.toLocal8Bit())).port(int_http_server_port).multithreaded().run_async();

					return a->exec();
				}
				else
				{
					qDebug() << "SimpleServers JsonArray is not correct";
					system("pause");
				}
			}
			else
			{
				qDebug() << "SimpleServer.json format is not correct";
				system("pause");

			}
		}
		else
		{
			qDebug() << "SimpleServer.json is not readable";
			system("pause");

		}
	}
	else
	{
		qDebug() << "SimpleServer.json is not exists";
		system("pause");

	}
}

QJsonObject ConvertStringToJson(QString* str_Json)
{
	QJsonObject json_Json;
	QList<QString> list_Request_str(str_Json->split('&'));
	foreach(QString i, list_Request_str)
	{
		json_Json.insert(QString(QUrl::fromPercentEncoding(i.split('=').first().toLocal8Bit())), QString(QUrl::fromPercentEncoding(i.split('=').last().toLocal8Bit())));
	}
	return json_Json;
}

QByteArray TestFunction(QByteArray* PostData, SimpleServers::SimpleServer* SServer)
{
	QJsonParseError t_pJsParseError{ QJsonParseError::NoError };
	QScopedPointer<QJsonDocument> t_pJsDoc{ new QJsonDocument{QJsonDocument::fromJson(*PostData, &t_pJsParseError)} };
	if (t_pJsParseError.error == QJsonParseError::NoError && t_pJsDoc->isObject())
	{
		QScopedPointer<QJsonObject> t_pJsonObject{ new QJsonObject{t_pJsDoc->object()} };
		QScopedPointer<QString> t_pstrSQLParameters{ new QString{""} };
		for (auto s : SServer->list_parameters)
		{
			if (!t_pstrSQLParameters->isEmpty())
			{
				*t_pstrSQLParameters += ", ";
			}
			else
			{
				*t_pstrSQLParameters += " (";
			}
			*t_pstrSQLParameters += '\'' + t_pJsonObject->value(s).toString() + '\'';
		}
		*t_pstrSQLParameters += ')';
		QScopedPointer<QString> String_SQL{ new QString(*SServer->bytearray_sql + *t_pstrSQLParameters) };

		QScopedPointer<QSqlQuery> query_Sql{ new QSqlQuery{*mysql->QueryExec(*String_SQL)} };
		QScopedPointer<QJsonObject> json_Test{ new QJsonObject() };
		QScopedPointer<QString> str_Json{ new QString() };

		*json_Test = {};

		QJsonArray JsonArray_Temp;

		//query_Sql = mysql->QueryExec(*String_SQL);

		while (query_Sql->next())
		{
			QJsonObject jsobjTemp = QJsonObject();
			for (int x{ 0 }; x < query_Sql->record().count(); ++x)
			{
				jsobjTemp.insert(query_Sql->record().fieldName(x), QJsonValue::fromVariant(query_Sql->value(x)));
			}
			JsonArray_Temp.push_back(jsobjTemp);
		}
		QScopedPointer<QJsonObject> t_pJsObj{ new QJsonObject };
		t_pJsObj->insert("data", JsonArray_Temp);
		return QJsonDocument{ *t_pJsObj }.toJson();
	}
	else
	{
		return QJsonDocument::fromJson("{\"Error\":\"Error\"}").toJson();
		qDebug() << "PostData format is not correct";
	}
}

// 处理主函数
void Logiccontroller(QByteArray* byteArr_Request, QByteArray* byteArr_ResponseHttp, QByteArray* byteArr_ResponseData)
{
	QScopedPointer<RequestParse> requestParsing{ new RequestParse };

	requestParsing->SetRequest(*byteArr_Request);

	//auto iter = map_Function->find(*requestParsing->GetParameter());
	QScopedPointer<QByteArray> Parameter{ new QByteArray{*requestParsing->GetParameter()} };
	bool flag = false;
	static SimpleServers::SimpleServer* simpleserver = new SimpleServers::SimpleServer;

	if (flag)
	{
		*byteArr_ResponseData = TestFunction(requestParsing->GetPostData(), simpleserver);
	}
	else if (!Parameter->compare("/vueWebServer"))
	{
		*byteArr_ResponseData = "{\"message\" : \"success\"}";
		*byteArr_ResponseHttp += "HTTP/1.1 200 OK\r\n";
	}
	else
	{
		*byteArr_ResponseHttp += "HTTP/1.1 404 NotFound\r\n";
	}

	*byteArr_ResponseHttp += "Server: nginx\r\n";
	//*byteArr_ResponseHttp += "Content-Type: text/html;charset=utf-8\r\n";
	*byteArr_ResponseHttp += "Content-Type: application/json; text/plain, charset=utf-8\r\n";
	*byteArr_ResponseHttp += "Connection: keep-alive\r\n";
	//*byteArr_ResponseHttp += "Access-controller-Allow-Origin: http://127.0.0.1:8080\r\n";
	*byteArr_ResponseHttp += "Access-controller-Allow-Origin: *\r\n";
	//*byteArr_ResponseHttp += "Access-controller-Allow-Methods: POST, OPTIONS\r\n";
	*byteArr_ResponseHttp += "Access-controller-Allow-Headers: Access-controller-Request-Headers, Authorization, Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, X-Requested-By, If-Modified-Since, X-File-Name, X-File-Type, Cache-controller, Origin, Access-Token\r\n";
	*byteArr_ResponseHttp += "Access-controller-Allow-Credentials: true\r\n";
	*byteArr_ResponseHttp += "access-controller-expose-headers: Authorization\r\n";
	*byteArr_ResponseHttp += "access-controller-expose-headers: *\r\n";
	*byteArr_ResponseHttp += "Access-controller-Max-Age: 60\r\n";
	*byteArr_ResponseHttp += "Vary: Accept-Encoding, Origin\r\n";
	*byteArr_ResponseHttp += "Keep-Alive: timeout=2, max=100\r\n";
	*byteArr_ResponseHttp += QString("Content-Length: %1\r\n\r\n").arg(QString::number((*byteArr_ResponseData).size())).toLocal8Bit();
}