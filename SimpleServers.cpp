#include "SimpleServers.h"

SimpleServers::SimpleServers()
= default;

void SimpleServers::InitSimpleServers()
{
	this->map_simple_servers_.clear();
}

void SimpleServers::InsertSimpleServer(const QByteArray& arg_bytearray_controller,
									   const QByteArray& arg_bytearray_method,
									   const QList<QByteArray>& arg_list_parameters,
									   const QByteArray& arg_bytearray_sql)
{
	//QByteArray QByteArray_MD5_Temp = QCryptographicHash::hash(*QByteArray_controller_Temp, QCryptographicHash::Md5).toHex();
	const SimpleServers::SimpleServer simple_servers{ arg_bytearray_method, arg_list_parameters, arg_bytearray_sql };
	if (!this->map_simple_servers_.contains(arg_bytearray_controller))
	{
		this->map_simple_servers_.insert(arg_bytearray_controller, simple_servers);
	}
	else
	{
		qDebug() << L"不能有两个相同的控制器 Can't add two identical controllers";
	}
}

void SimpleServers::EraseSimpleServer()
{

}

void SimpleServers::InitSimpleServersFromJson(const QJsonArray& arg_json_array)
{
	this->InitSimpleServers();

	std::ranges::for_each(arg_json_array, [this](const QJsonValue& temp_json_value)
	{
		QJsonObject json_object = temp_json_value.toObject();

		const QByteArray bytearray_controller{ json_object.take("Controller").toString().toLocal8Bit() };

		QList<QByteArray> list_parameters{};

		Q_FOREACH(const QString & temp_string, json_object.take("Parameters").toString().split(','))
		{
			list_parameters.emplace_back(temp_string.trimmed().toLocal8Bit());
		}

		const QByteArray bytearray_sql{ json_object.take("SQLString").toString().toLocal8Bit() };

		const QByteArray bytearray_method{ json_object.take("Method").toString().toLocal8Bit() };

		this->InsertSimpleServer(bytearray_controller, bytearray_method, list_parameters, bytearray_sql);
	});
}

QMap<QByteArray, SimpleServers::SimpleServer> SimpleServers::GetSimpleServersMap()
{
	return map_simple_servers_;
}

void SimpleServers::Run()
{

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
				/*mysql = new Mysql(json_object_mysql.value("Host").toString(),
								  json_object_mysql.value("Port").toString().toInt(),
								  json_object_mysql.value("DataBase").toString(),
								  json_object_mysql.value("UserName").toString(),
								  json_object_mysql.value("Password").toString(),
								  "db_connect");*/

				if (const QJsonDocument json_document_simple_server{ QJsonDocument{json_object_simple_server.value("SimpleServers").toArray()} }; json_document_simple_server.array().count() > 0)
				{
					qDebug() << "SimpleServers JsonArray is correct";
					const QJsonArray json_array_simple_server{ QJsonArray{json_document_simple_server.array()} };
					uint_http_server_port_ = json_object_simple_server.value("HttpServerPort").toString().toUInt();
					bytearray_http_server_ip_address_ = json_object_simple_server.value("HttpServerIpAddress").toString().toLocal8Bit();
					unit_http_server_log_level_ = json_object_simple_server.value("HttpServerLogLevel").toString().toUInt();

					// Init SimpleServers
					InitSimpleServersFromJson(json_array_simple_server);

					// 连接数据库
					//mysql->connect();

					// Init Crow
					LogHelperHandler handler_log_helper;
					crow::logger::setHandler(&handler_log_helper);

					crow::App<SimpleServerMiddleware> simple_app_crow{};
					//simple_app_crow.get_middleware<SimpleServerMiddleware>().SetMessage("Hello world");
					auto temp = GetSimpleServersMap().keys();
					std::ranges::for_each(GetSimpleServersMap().keys(), [this, &simple_app_crow](const QByteArray& temp_bytearray_key)
					{
						const QString string_method = GetSimpleServersMap().value(temp_bytearray_key).method;
						const std::vector<QString> vector_method_strings{ SimpleServers::GetVectorMethodStrings() };
						const auto iterator_vector_method_strings =
							std::ranges::find(vector_method_strings, string_method);
						const auto int_index_vector_method_strings = std::distance(
							vector_method_strings.begin(), iterator_vector_method_strings);
						simple_app_crow.route_dynamic(static_cast<std::string>(temp_bytearray_key)).methods(static_cast<crow::HTTPMethod>(int_index_vector_method_strings))([](const crow::request& request_request)
						{
							auto a = method_name(request_request.method);
							const QString string_temp = R"({"method":")" + QString::fromLocal8Bit(a.data()) +R"("})";
							return static_cast<std::string>(string_temp.toLocal8Bit());
						});
					});
					simple_app_crow.loglevel(static_cast<crow::LogLevel>(unit_http_server_log_level_));
					auto sync_app_crow = simple_app_crow.bindaddr(static_cast<std::string>(bytearray_http_server_ip_address_)).port(uint_http_server_port_).multithreaded().run_async();

					qDebug() << "Init Crow Completed";
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
