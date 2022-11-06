#include "simple_servers.h"

SimpleServers::SimpleServers(QObject* parent) : QObject(parent)
{
}

void SimpleServers::InitSQL()
{
	// Mysql sql_server{"QODBC", "172.28.99.74", 1433, "CAPS_DEV", "svc_portal_crm", "K97a1pBsvGk8xly6U", "db1"};
	sql_server.connect();
	QSqlQuery query_Sql = sql_server.QueryExec("SELECT TOP 1 * FROM tblConfig with(nolock)");
	query_Sql.setForwardOnly(true);
	QJsonArray JsonArray_Temp;

	while (query_Sql.next())
	{
		for (int x{0}; x < query_Sql.record().count(); ++x)
		{
			JsonArray_Temp.push_back(QJsonObject{{query_Sql.record().fieldName(x), QJsonValue::fromVariant(query_Sql.value(x))}});
		}
	}

	qDebug() << QJsonDocument{JsonArray_Temp}.toJson();
}

void SimpleServers::InitSimpleServers(const QJsonObject& arg_json_object)
{
	json_object_simple_server_ = arg_json_object;
}

void SimpleServers::init_sql_connect_by_json_object(const QJsonObject& arg_json_object_sql_servers)
{
	json_object_sql_servers_ = arg_json_object_sql_servers;
	for (QJsonObject::const_iterator it = json_object_sql_servers_.constBegin(); it != json_object_sql_servers_.constEnd(); ++it)
	{
		QJsonObject temp_json_object_sql = it.value().toObject();
		ConnectionPoolSimple::StructSqlServer temp_sql_server
		{
			it.key(),
			temp_json_object_sql.value("SQLDriver").toString(),
			temp_json_object_sql.value("Host").toString(),
			temp_json_object_sql.value("Port").toString(),
			temp_json_object_sql.value("UserName").toString(),
			temp_json_object_sql.value("Password").toString(),
			temp_json_object_sql.value("DataBase").toString()
		};

		map_sql_servers_.insert(it.key(), temp_sql_server);
	}
}

void SimpleServers::Run()
{
	// InitSQL();

	LogHelperHandler handler_log_helper{};
	crow::logger::setHandler(&handler_log_helper);

	crow::App<SimpleServerMiddleware> simple_app_crow{};
	simple_app_crow.loglevel(static_cast<crow::LogLevel>(json_object_simple_server_.value("SimpleServerLogLevel").toString().toInt()));

	// CROW_CATCHALL_ROUTE(simple_app_crow);
	const QJsonObject json_object_controllers{json_object_simple_server_.value("Controllers").toObject()};
	simple_app_crow.catchall_route()([&](const crow::request& request_request)
	{
		crow::response response_response{};
		response_response.code = 200;
		response_response.body = "";

		//response_response.body = QJsonDocument{JsonArray_Temp}.toJson();
		//return response_response;

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
							std::ranges::for_each(arg_json_object_child.keys(), [&](const QString& temp_bytearray_child_key) -> void
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
									if (arg_json_object_child.value(temp_bytearray_child_key).isObject())
									{
										QJsonObject json_object_child_value = arg_json_object_child.value(temp_bytearray_child_key).toObject();
										function_resolve_data_mapping(arg_json_object_child, json_object_child_value);
										/*std::ranges::for_each(json_object_child_value.keys(), [&](const QString& temp_string_key)-> void
										{
											arg_json_object_child.insert(temp_bytearray_child_key, json_object_child_value.value(temp_string_key));
										});*/
										arg_json_object_child.insert(temp_bytearray_child_key, json_object_child_value);
									}
									else
									{
										QString copy_string_value{arg_json_object_child.value(temp_bytearray_child_key).toString()};
										for (const QRegularExpressionMatch& temp_regex_match : regexp.globalMatch(copy_string_value))
										{
											if (QJsonValue json_value_data{arg_json_object_child.value(temp_bytearray_child_key)};
												json_value_data.isObject())
											{
												//QJsonObject temp_json_object_child_child{json_value_data.toObject()};
												//function_resolve_data_mapping(arg_json_object_child, temp_json_object_child_child);
												//arg_json_object_child.insert(temp_bytearray_child_key, temp_json_object_child_child);
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
														copy_string_value.replace(temp_regex_match.captured(), CommonTools::ConvertImgToBase64(bytearray_data));
													}
												}
												else if (!bytearray_data.compare("{GUID}") || !bytearray_data.compare("{UUID}"))
												{
													QUuid uuid_uuid{QUuid::createUuid()};
													QString string_base_data{request_request.remote_ip_address.data()};
													copy_string_value.replace(temp_regex_match.captured(), QUuid::createUuidV5(uuid_uuid, string_base_data).toString());
												}
												else if (!bytearray_data.compare("{CaptchaImage}"))
												{
													QByteArray bytearray_base64{CommonTools::ConvertMatToBase64(CommonTools::CreateCaptchaImage())};
													copy_string_value.replace(temp_regex_match.captured(), bytearray_base64);
												}
												else if (bytearray_data.indexOf('.') != -1)
												{
													QByteArrayList bytearray_list_sql_name{QByteArrayList::fromList(bytearray_data.replace('{', "").replace('}', "").split('.'))};
													if (QJsonObject json_object_sql_servers{json_object_controller.value("SQL").toObject()}; json_object_sql_servers.contains(bytearray_list_sql_name.first()))
													{
														const QJsonObject temp_json_object_sql{json_object_sql_servers.value(bytearray_list_sql_name.first()).toObject()};

														// [1] 从数据库连接池里取得连接
														QSqlDatabase db = ConnectionPoolSimple::openConnection(map_sql_servers_.value(temp_json_object_sql.value("SqlName").toString()));

														// [2] 使用连接查询数据库
														QSqlQuery query(db);

														query.setForwardOnly(true);
														query.exec(temp_json_object_sql.value("SqlQuery").toString());
														QJsonArray json_array_temp{};

														while (query.next())
														{
															for (int x{0}; x < query.record().count(); ++x)
															{
																json_array_temp.push_back(QJsonObject{{query.record().fieldName(x), QJsonValue::fromVariant(query.value(x))}});
															}
														}
														copy_string_value.replace(temp_regex_match.captured(), QJsonDocument{json_array_temp}.toJson());
													}
												}
											}
										}
										QJsonParseError error_json_parse{QJsonParseError::NoError};
										const QJsonDocument json_document_copy_string_value{QJsonDocument::fromJson(copy_string_value.toLocal8Bit(), &error_json_parse)};
										if (error_json_parse.error == QJsonParseError::NoError)
										{
											if (json_document_copy_string_value.isArray())
											{
												arg_json_object_child.insert(temp_bytearray_child_key, json_document_copy_string_value.array());
											}
											else if (json_document_copy_string_value.isObject())
											{
												arg_json_object_child.insert(temp_bytearray_child_key, json_document_copy_string_value.object());
											}
										}
										else
										{
											arg_json_object_child.insert(temp_bytearray_child_key, copy_string_value);
										}
									}
								}
							});
						};
						function_resolve_data_mapping(json_object_controller, json_object_data);

						// SQL
						// QJsonObject json_object_SQL{json_object_controller.value("SQL").toObject()};

						std::ranges::for_each(json_object_response.keys(), [&](const QString& temp_string_key) -> void
						{
							QString temp_string_value{json_object_response.value(temp_string_key).toString()};
							for (const QRegularExpression regexp{R"({(.*?)})"};
							     const QRegularExpressionMatch& temp_regex_match : regexp.globalMatch(temp_string_value))
							{
								QJsonObject copy_json_object_data{json_object_data};
								QString out_temp_string{};
								bool bool_is_json_object{true};
								bool bool_is_success{true};
								for (const QString& temp_string : temp_regex_match.captured(1).split('.'))
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
									if (copy_json_object_data.value(out_temp_string).isObject())
									{
										temp_string_value.replace(temp_regex_match.captured(), QJsonDocument{copy_json_object_data.value(out_temp_string).toObject()}.toJson());
									}
									else if (copy_json_object_data.value(out_temp_string).isArray())
									{
										temp_string_value.replace(temp_regex_match.captured(), QJsonDocument{copy_json_object_data.value(out_temp_string).toArray()}.toJson());
									}
									else
									{
										temp_string_value.replace(temp_regex_match.captured(), copy_json_object_data.value(out_temp_string).toString());
									}
								}
							}
							//json_object_response.insert(temp_string_key, temp_string_value);
							QJsonParseError error_json_parse{QJsonParseError::NoError};
							const QJsonDocument json_document_copy_string_value{QJsonDocument::fromJson(temp_string_value.toLocal8Bit(), &error_json_parse)};
							if (error_json_parse.error == QJsonParseError::NoError)
							{
								if (json_document_copy_string_value.isArray())
								{
									json_object_response.insert(temp_string_key, json_document_copy_string_value.array());
								}
								else if (json_document_copy_string_value.isObject())
								{
									json_object_response.insert(temp_string_key, json_document_copy_string_value.object());
								}
							}
							else
							{
								json_object_response.insert(temp_string_key, temp_string_value);
							}
						});
					}
					response_response.body = static_cast<std::string>(QJsonDocument{json_object_response}.toJson());
				}
			}
		}
		return response_response;
	});

	auto sync_app_crow =
		simple_app_crow.bindaddr(json_object_simple_server_.value("SimpleServerIpAddress")
		                                                   .toString()
		                                                   .toLocal8Bit()
		                                                   .data())
		               .port(json_object_simple_server_.value("SimpleServerPort")
		                                               .toString()
		                                               .toUInt())
		               .multithreaded()
		               .run_async();
}

void SimpleServers::emit_run()
{
	emit signal_run();
}
