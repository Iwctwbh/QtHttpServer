#include "simple_servers.h"

SimpleServers::SimpleServers(QObject *parent) : QObject(parent)
{
}

void SimpleServers::InitSimpleServers(const QJsonObject &arg_json_object)
{
	json_object_simple_server_ = arg_json_object;
}

void SimpleServers::Run() const
{
	const QJsonObject json_object_simple_server = json_object_simple_server_;

	LogHelperHandler handler_log_helper{};
	crow::logger::setHandler(&handler_log_helper);

	crow::App<SimpleServerMiddleware> simple_app_crow{};
	simple_app_crow.loglevel(static_cast<crow::LogLevel>(json_object_simple_server.value("SimpleServerLogLevel").toString().toInt()));

	//CROW_CATCHALL_ROUTE(simple_app_crow);
	QJsonObject json_object_controllers{ json_object_simple_server.value("Controllers").toObject() };
	simple_app_crow.catchall_route()([this, &json_object_controllers](const crow::request &request_request)
	{
		crow::response response_response{};
		if (!json_object_controllers.contains(request_request.url.data()))
		{
			response_response.code = 404;
		}
		else
		{
			if (const QJsonObject json_object_controller{ json_object_controllers.value(request_request.url.data()).toObject() }; !json_object_controller.value("Methods").toString().toUpper().replace(" ", "").split(',').contains(crow::method_name(request_request.method).data()))
			{
				response_response.code = 404;
			}
			else
			{
				if (!json_object_controller.contains("Response"))
				{
					response_response.code = 200;
				}
				else
				{
					QJsonObject json_object_response{ json_object_controller.value("Response").toObject() };
					if (json_object_response.isEmpty())
					{
						response_response.code = 200;
					}
					else
					{
						std::ranges::for_each(json_object_response.keys(), [this, &json_object_response, &request_request, &json_object_controller](const QString &temp_bytearray_key)
						{
							const QByteArray temp_byte_array = json_object_response.value(temp_bytearray_key).toString().toLocal8Bit();
							const QRegularExpression regexp{ R"({(\w+)})" };
							for (const QRegularExpressionMatch &temp_regex_match : regexp.globalMatch(temp_byte_array))
							{
								QByteArray bytearray_data = json_object_controller.value("data").toObject().value(temp_regex_match.captured(1)).toString().toLocal8Bit();

								if (const QFile temp_file{ bytearray_data }; temp_file.exists())
								{
									if (const QMimeType temp_mimetype{ QMimeDatabase{}.mimeTypeForFile(bytearray_data) }; temp_mimetype.name().startsWith("image/"))
									{
										json_object_response.insert(temp_bytearray_key, QtCommonTools::ConvertImgToBase64(bytearray_data).data());
									}
								}
								else if (bytearray_data == "{GUID}")
								{
									QUuid uuid_id = QUuid::createUuid();
									QString string_base_data{ request_request.remote_ip_address.data() };
									json_object_response.insert(temp_bytearray_key, QUuid::createUuidV5(uuid_id, string_base_data).toString());
								}
							}
						});
					}
					response_response.body = static_cast<std::string>(QJsonDocument{ json_object_response }.toJson());
				}
			}
		}
		return response_response;
	});

	auto sync_app_crow = simple_app_crow.bindaddr(static_cast<std::string>(json_object_simple_server.value("SimpleServerIpAddress").toString().toLocal8Bit())).port(json_object_simple_server.value("SimpleServerPort").toString().toUInt()).multithreaded().run_async();
}

void SimpleServers::emit_run()
{
	emit signal_run();
}
