//#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>
#include <QtSql>

#include "mysql.h"
#include "SimpleServers.h"
#include "drogon/drogon.h"
#include "crow.h"

void Logiccontroller(QByteArray* byteArr_Request, QByteArray* byteArr_ResponseHttp, QByteArray* byteArr_ResponseData);

Mysql* mysql;

constexpr std::vector<QString> GetVectorMethodStrings()
{
	std::vector<QString> list_method_strings{};
	std::ranges::for_each(crow::method_strings, [&](const char* temp_ptr_char)
	{
		list_method_strings.emplace_back(temp_ptr_char);
	});
	return list_method_strings;
}

class LogHelperHandler final : public crow::CerrLogHandler
{
public:
	auto log(const std::string message, const crow::LogLevel level) -> void override
	{
		std::string prefix;
		switch (level)
		{
			case crow::LogLevel::Debug:
				prefix = "DEBUG   ";
				break;
			case crow::LogLevel::Info:
				prefix = "INFO    ";
				break;
			case crow::LogLevel::Warning:
				prefix = "WARNING ";
				break;
			case crow::LogLevel::Error:
				prefix = "ERROR   ";
				break;
			case crow::LogLevel::Critical:
				prefix = "CRITICAL";
				break;
		}
		std::cerr << "(" << static_cast<std::string>(QDateTime::currentDateTime().toString().toLocal8Bit()) << ") [" << prefix << "] " << message << std::endl;
	}
};

struct SimpleServerMiddleware : crow::ILocalMiddleware
{
	QString string_message{};

	void SetMessage(const QString& arg_message)
	{
		string_message = arg_message;
	}

	struct context
	{
	};

	auto before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) const -> void
	{
		//CROW_LOG_DEBUG << " - MESSAGE: " << string_message;
	}

	auto after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) -> void
	{
		// no-op
		//CROW_LOG_DEBUG << " - END";
	}
};

int main(int argc, char* argv[])
{
	[[maybe_unused]] const QCoreApplication* a = new QCoreApplication{ argc, argv };

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
					const uint16_t uint_http_server_port = json_object_simple_server.value("HttpServerPort").toString().toUInt();
					const QByteArray bytearray_http_server_ip_address = json_object_simple_server.value("HttpServerIPAddress").toString().toLocal8Bit();
					const uint16_t unit_http_server_log_level = json_object_simple_server.value("HttpServerLogLevel").toString().toUInt();

					// Init SimpleServers
					SimpleServers simple_servers{};
					simple_servers.InitSimpleServersFromJson(json_array_simple_server);

					// 连接数据库
					mysql->connect();

					// Init Crow
					LogHelperHandler handler_log_helper;
					crow::logger::setHandler(&handler_log_helper);

					crow::App<SimpleServerMiddleware> simple_app_crow{};
					//simple_app_crow.get_middleware<SimpleServerMiddleware>().SetMessage("Hello world");
					std::ranges::for_each(simple_servers.GetSimpleServersMap().keys(), [&simple_app_crow, &simple_servers](const QByteArray& temp_bytearray_key)
					{
						const QString string_method = simple_servers.GetSimpleServersMap().value(temp_bytearray_key).method;
						const std::vector<QString> vector_method_strings{ GetVectorMethodStrings() };
						const auto iterator_vector_method_strings =
							std::ranges::find(vector_method_strings, string_method);
						const auto int_index_vector_method_strings = std::distance(
							vector_method_strings.begin(), iterator_vector_method_strings);
						simple_app_crow.route_dynamic(static_cast<std::string>(temp_bytearray_key)).methods(static_cast<crow::HTTPMethod>(int_index_vector_method_strings))([](const crow::request& request_request)
						{
							auto a = method_name(request_request.method);
							auto json_string = request_request.body;
							return a;
						});
					});
					simple_app_crow.loglevel(crow::LogLevel::Info);
					auto sync_app_crow = simple_app_crow.bindaddr(static_cast<std::string>(bytearray_http_server_ip_address)).port(uint_http_server_port).multithreaded().run_async();

					qDebug() << "Init Crow Completed";

					return QCoreApplication::exec();
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
