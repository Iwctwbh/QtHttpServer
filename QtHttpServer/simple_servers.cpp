#include "simple_servers.h"

SimpleServers::SimpleServers(QObject* parent) : QObject(parent)
{
}

void SimpleServers::InitSimpleServers(const QJsonObject& arg_json_object)
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

	// CROW_CATCHALL_ROUTE(simple_app_crow);
	const QJsonObject json_object_controllers{json_object_simple_server.value("Controllers").toObject()};
	simple_app_crow.catchall_route()([&](const crow::request& request_request)
	{
		crow::response response_response{};
		if (!json_object_controllers.contains(request_request.url.data()))
		{
			response_response.code = 404;
		}
		else
		{
			if (QJsonObject json_object_controller{json_object_controllers.value(request_request.url.data()).toObject()};
				!json_object_controller.value("Methods")
				                       .toString()
				                       .toUpper()
				                       .replace(" ", "")
				                       .split(',')
				                       .contains(method_name(request_request.method).data()))
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
					QJsonObject json_object_response{json_object_controller.value("Response").toObject()};
					if (json_object_response.isEmpty())
					{
						response_response.code = 200;
					}
					else
					{
						QJsonObject json_object_data{json_object_controller.value("data").toObject()};
						std::function<void(QJsonObject& arg_json_object_parent, QJsonObject& arg_json_object_child)> function_resolve_data_mapping = [&](QJsonObject& arg_json_object_parent, QJsonObject& arg_json_object_child)-> void
						{
							std::ranges::for_each(arg_json_object_child.keys(),
							                      [&](const QString& temp_bytearray_child_key) -> void
							                      {
								                      if (const QRegularExpression regexp{R"({(.*?)})"};
									                      regexp.match(temp_bytearray_child_key).hasMatch())
								                      {
									                      QJsonObject temp_json_object_child_child{arg_json_object_child.value(temp_bytearray_child_key).toObject()};
									                      function_resolve_data_mapping(arg_json_object_child, temp_json_object_child_child);
									                      std::ranges::for_each(temp_json_object_child_child.keys(), [&](const QString& temp_string_key)-> void
									                      {
										                      arg_json_object_child.insert(temp_string_key, temp_json_object_child_child.value(temp_string_key));
									                      });
									                      arg_json_object_child.remove(temp_bytearray_child_key);
								                      }
								                      else
								                      {
									                      for (QString temp_string_value{arg_json_object_child.value(temp_bytearray_child_key).toString()}; const QRegularExpressionMatch& temp_regex_match : regexp.globalMatch(
										                           temp_string_value))
									                      {
										                      if (QJsonValue json_value_data{json_object_data.value(temp_regex_match.captured(1))};
											                      json_value_data.isObject())
										                      {
											                      QJsonObject temp_json_object_child_child{json_value_data.toObject()};
											                      function_resolve_data_mapping(arg_json_object_child, temp_json_object_child_child);
											                      arg_json_object_child.insert(temp_bytearray_child_key, temp_json_object_child_child);
										                      }
										                      else
										                      {
											                      QByteArray bytearray_data{json_value_data.toString().toLocal8Bit()};

											                      if (const QFile temp_file{bytearray_data};
												                      temp_file.exists())
											                      {
												                      if (const QMimeType temp_mimetype{QMimeDatabase{}.mimeTypeForFile(bytearray_data)};
													                      temp_mimetype.name().startsWith("image/"))
												                      {
													                      temp_string_value.replace(temp_regex_match.captured(), CommonTools::ConvertImgToBase64(bytearray_data));
												                      }
											                      }
											                      else if (!bytearray_data.compare("{GUID}") || !bytearray_data.compare("{UUID}"))
											                      {
												                      QUuid uuid_uuid{QUuid::createUuid()};
												                      QString string_base_data{request_request.remote_ip_address.data()};
												                      temp_string_value.replace(temp_regex_match.captured(), QUuid::createUuidV5(uuid_uuid, string_base_data).toString());
											                      }
											                      else if (!bytearray_data.compare("{CaptchaImage}"))
											                      {
												                      QByteArray bytearray_base64{CommonTools::ConvertMatToBase64(CommonTools::CreateCaptchaImage())};
												                      temp_string_value.replace(temp_regex_match.captured(), bytearray_base64);
											                      }
										                      }
									                      }
								                      }
							                      });
						};
						function_resolve_data_mapping(json_object_controller, json_object_data);
						std::ranges::for_each(json_object_response.keys(), [&](const QString& temp_string_key)-> void
						{
							QString temp_string_value{json_object_response.value(temp_string_key).toString()};
							for (const QRegularExpression regexp{R"({(.*?)})"};
							     const QRegularExpressionMatch& temp_regex_match : regexp.globalMatch(temp_string_value))
							{
								QJsonObject copy_json_object_data{json_object_data};
								QString out_temp_string{};
								bool bool_is_json_object{true};
								bool bool_is_success{true};
								foreach(const QString& temp_string, temp_regex_match.captured(1).split('.'))
								{
									out_temp_string = temp_string;
									if (bool_is_json_object)
									{
										if (copy_json_object_data.contains(temp_string))
										{
											if (copy_json_object_data.value(temp_string).isObject())
											{
												copy_json_object_data = copy_json_object_data.value(temp_string).toObject();
											}
											else
											{
												bool_is_json_object = false;
											}
										}
									}
									else
									{
										bool_is_success = false;
										break;
									}
								}
								if (bool_is_success)
								{
									temp_string_value.replace(temp_regex_match.captured(1), copy_json_object_data.value(out_temp_string).toString());
								}
							}
							json_object_response.insert(temp_string_key,
							                            temp_string_value);
						});
					}
					response_response.body = static_cast<std::string>(QJsonDocument{json_object_response}.toJson());
				}
			}
		}
		return response_response;
	});

	auto sync_app_crow =
		simple_app_crow.bindaddr(json_object_simple_server.value("SimpleServerIpAddress")
		                                                  .toString()
		                                                  .toLocal8Bit()
		                                                  .data())
		               .port(json_object_simple_server.value("SimpleServerPort")
		                                              .toString()
		                                              .toInt())
		               .multithreaded()
		               .run_async();
}

void SimpleServers::emit_run()
{
	emit signal_run();
}
