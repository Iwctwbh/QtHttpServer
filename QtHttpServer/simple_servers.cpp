#include "simple_servers.h"

SimpleServers::SimpleServers(QObject *parent) : QObject(parent)
{

}

void SimpleServers::InitSimpleServers()
{
	map_simple_servers_.clear();
}

void SimpleServers::InsertSimpleServer(const QByteArray &arg_bytearray_controller,
									   const QByteArray &arg_bytearray_method,
									   const QList<QByteArray> &arg_list_parameters,
									   const QByteArray &arg_bytearray_sql,
									   const QJsonObject &arg_json_object_data,
									   const QJsonObject &arg_bytearray_response)
{
	//QByteArray QByteArray_MD5_Temp = QCryptographicHash::hash(*QByteArray_controller_Temp, QCryptographicHash::Md5).toHex();
	const SimpleServer simple_servers{ arg_bytearray_method, arg_list_parameters, arg_bytearray_sql, arg_json_object_data, arg_bytearray_response };
	if (!map_simple_servers_.contains(arg_bytearray_controller))
	{
		map_simple_servers_.insert(arg_bytearray_controller, simple_servers);
	}
	else
	{
		qDebug() << L"不能有两个相同的控制器 Can't add two identical controllers";
	}
}

void SimpleServers::EraseSimpleServer()
{

}

void SimpleServers::InitSimpleServersFromJson(const QJsonArray &arg_json_array)
{
	InitSimpleServers();

	std::ranges::for_each(arg_json_array, [this](const QJsonValue &temp_json_value)
	{
		QJsonObject temp_json_object = temp_json_value.toObject();

		const QByteArray bytearray_controller{ temp_json_object.take("Controller").toString().toLocal8Bit() };

		QList<QByteArray> list_parameters{};

		foreach(const QString & temp_string, temp_json_object.take("Parameters").toString().split(','))
		{
			list_parameters.emplace_back(temp_string.trimmed().toLocal8Bit());
		}

		const QByteArray bytearray_sql{ temp_json_object.take("SQLString").toString().toLocal8Bit() };

		const QByteArray bytearray_method{ temp_json_object.take("Method").toString().toLocal8Bit() };

		const QJsonObject json_object_data{ temp_json_object.value("data").toObject() };

		const QJsonObject json_object_response{ temp_json_object.take("Response").toObject() };

		InsertSimpleServer(bytearray_controller, bytearray_method, list_parameters, bytearray_sql, json_object_data, json_object_response);
	});
}

QHash<QByteArray, SimpleServers::SimpleServer> &SimpleServers::GetSimpleServersMap()
{
	return map_simple_servers_;
}

void SimpleServers::Run()
{

	if (QFile file_simple_server{ "SimpleServer.json" }; file_simple_server.exists())
	{
		qDebug() << "SimpleServer.json exists";
		//qDebug() << "SimpleServer.json is readable";
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

				if (const QJsonDocument json_document_simple_server{ json_object_simple_server.value("SimpleServers").toArray() }; json_document_simple_server.array().count() > 0)
				{
					qDebug() << "SimpleServers JsonArray is correct";
					const QJsonArray json_array_simple_server{ json_document_simple_server.array() };
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

					std::ranges::for_each(map_simple_servers_.keys(), [this, &simple_app_crow](const QByteArray &temp_bytearray_key)
					{
						const QByteArray string_method = map_simple_servers_.value(temp_bytearray_key).method;
						const std::vector<QByteArray> vector_method_strings{ GetVectorMethodStrings() };
						const auto iterator_vector_method_strings =
							std::ranges::find(vector_method_strings, string_method);
						const auto int_index_vector_method_strings = std::distance(
							vector_method_strings.begin(), iterator_vector_method_strings);
						simple_app_crow.route_dynamic(static_cast<std::string>(temp_bytearray_key)).methods(static_cast<crow::HTTPMethod>(int_index_vector_method_strings))([this](const crow::request &request_request)
						{
							if (QJsonObject json_object_response{ map_simple_servers_.value(request_request.url.data()).json_object_response }; !json_object_response.isEmpty())
							{
								std::ranges::for_each(json_object_response.keys(), [this, &json_object_response, &request_request](const QString &temp_bytearray_key)
								{
									const QByteArray temp_byte_array = json_object_response.value(temp_bytearray_key).toString().toLocal8Bit();
									const QRegularExpression regexp{ R"({(\w+)})" };
									for (const QRegularExpressionMatch &temp_regex_match : regexp.globalMatch(temp_byte_array))
									{
										auto a = map_simple_servers_.value(request_request.url.data());
										auto b = temp_regex_match.captured(1);
										auto c = a.json_object_data.value(b);
										QByteArray bytearray_data = map_simple_servers_.value(request_request.url.data()).json_object_data.value(temp_regex_match.captured(1)).toString().toLocal8Bit();

										if (const QFile temp_file{ bytearray_data }; temp_file.exists())
										{
											if (const QMimeType temp_mimetype{ QMimeDatabase{}.mimeTypeForFile(bytearray_data) }; temp_mimetype.name().startsWith("image/"))
											{
												json_object_response.insert(temp_bytearray_key, QtCommonTools::ConvertImgToBase64(bytearray_data).data());
											}
										}
									}
								});

								return static_cast<std::string>(QJsonDocument{ json_object_response }.toJson());
							}
							const QString string_temp = R"({"method":")" + QString::fromLocal8Bit(method_name(request_request.method)) + R"("})";
							return static_cast<std::string>(string_temp.toLocal8Bit());
						});
					});

					if (json_object_simple_server.contains("WebController") && json_object_simple_server.value("WebController").isObject())
					{
						if (const QJsonObject json_object_web_controller{ json_object_simple_server.value("WebController").toObject() }; json_object_web_controller.value("WebServer").toString().compare("true"))
						{
							simple_app_crow.route_dynamic(static_cast<std::string>(json_object_web_controller.value("WebServerController").toString().toLocal8Bit())).methods(crow::HTTPMethod::POST)([this](const crow::request &request_request)
							{
								const QString string_temp = R"({"method":")" + QString::fromLocal8Bit(method_name(request_request.method)) + R"("})";
								return static_cast<std::string>(string_temp.toLocal8Bit());
							});
						}
					}
					else
					{
						qDebug() << "No Web Controller";
					}

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

void SimpleServers::emit_run()
{
	emit signal_run();
}
