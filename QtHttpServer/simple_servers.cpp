#include "simple_servers.h"

static const QRegularExpression kRegexp{R"({(.*)})"};
static const QRegularExpression kSqlRegexp{R"({(.*?)})"};

SimpleServers::SimpleServers(QObject* parent) : QObject(parent)
{
}

void SimpleServers::InitSimpleServers(const QJsonObject& arg_json_object)
{
	string_ip_address_ = arg_json_object.value("SimpleServerIpAddress")
	                                    .toString()
	                                    .toLocal8Bit()
	                                    .data();

	int_port_ = arg_json_object.value("SimpleServerPort")
	                           .toString()
	                           .toUInt();

	int_log_level_ = arg_json_object.value("SimpleServerLogLevel").toString().toUInt();

	json_object_controllers_ = arg_json_object.value("Controllers").toObject();
}

void SimpleServers::Run() const
{
	LogHelperHandler handler_log_helper{};
	crow::logger::setHandler(&handler_log_helper);

	crow::App<SimpleServerMiddleware> simple_app_crow{};
	simple_app_crow.loglevel(static_cast<crow::LogLevel>(int_log_level_));

	// CROW_CATCHALL_ROUTE(simple_app_crow);
	const Controllers controllers{json_object_controllers_};
	simple_app_crow.catchall_route()([&](const crow::request& request_request)
	{
		crow::response response_response{};
		response_response.code = 200;
		response_response.body = "";

		Controller controller = controllers.GetController(request_request.url.data());

		if (QString temp_method{method_name(request_request.method).data()};
			controller.IsInMethods(temp_method))
		{
			response_response.code = 404;
		}
		else
		{
			if (controller.IsResponseEmpty())
			{
				// send to thread and return
			}

			controller.MappingData();

			QJsonObject temp_json_object{};
			const QHash<QString, QString> copy_hash_response = controller.GetResponse();
			for (auto it{copy_hash_response.constBegin()}; it != copy_hash_response.constEnd(); ++it)
			{
				temp_json_object.insert(it.key(), it.value());
			}

			response_response.body = static_cast<std::string>(QJsonDocument{temp_json_object}.toJson());
		}

		return response_response;
	});

	auto sync_app_crow =
		simple_app_crow.bindaddr(string_ip_address_.data())
		               .port(int_port_)
		               .multithreaded()
		               .run_async();
}

void SimpleServers::emit_run()
{
	emit signal_run();
}

Controllers::Controllers(const QJsonObject& arg_json_object)
{
	for (auto it = arg_json_object.constBegin(); it != arg_json_object.constEnd(); ++it)
	{
		controllers_.insert(it.key(), Controller(it.value().toObject()));
	}
}

Controller Controllers::GetController(const QString& arg_name) const
{
	if (controllers_.contains(arg_name))
	{
		return controllers_.value(arg_name);
	}
	Controller temp_controller{};
	return temp_controller;
}

Controller::Controller(const QJsonObject& arg_json_object)
{
	// methods
	std::ranges::for_each(
		arg_json_object.value("Methods")
		               .toString()
		               .toUpper()
		               .replace(" ", "")
		               .split(','),
		[&](const QString& s)
		{
			set_methods_.insert(s);
		});

	// parameters
	const QJsonObject json_object_parameters = arg_json_object.value("Parameters").toObject();
	for (auto it = json_object_parameters.constBegin(); it != json_object_parameters.constEnd(); ++it)
	{
		hash_parameters_.insert(it.key(), "");
	}

	// SQL
	const QJsonObject json_object_sql_servers = arg_json_object.value("SQL").toObject();
	for (auto it = json_object_sql_servers.constBegin(); it != json_object_sql_servers.constEnd(); ++it)
	{
		const QJsonObject json_object_sql = it->toObject();
		const QJsonObject json_object_sql_data = json_object_sql.value("data").toObject();
		/*QHash<QString, QString> hash_data{};
		for (auto it_data = json_object_sql_data.constBegin(); it_data != json_object_sql_data.constEnd(); ++it_data)
		{
			hash_data.insert(it_data.key(), it.value().toString());
		}*/

		auto sql = Sql{
			json_object_sql.value("SqlName").toString(),
			json_object_sql.value("SqlQuery").toString(),
			""
		};

		hash_sql_.insert(it.key(), sql);
	}

	// data
	const QJsonObject json_object_data = arg_json_object.value("data").toObject();
	for (auto it = json_object_data.constBegin(); it != json_object_data.constEnd(); ++it)
	{
		hash_data_.insert(it.key(), it.value().toString());
	}

	// Response
	const QJsonObject json_object_response = arg_json_object.value("Response").toObject();
	for (auto it = json_object_response.constBegin(); it != json_object_response.constEnd(); ++it)
	{
		hash_response_.insert(it.key(), it.value().toString());
	}
}

Controller::Controller()
{
}

bool Controller::IsInMethods(const QString& arg_method) const
{
	return hash_parameters_.contains(arg_method);
}

bool Controller::IsResponseEmpty() const
{
	return hash_response_.isEmpty();
}

void Controller::MappingData()
{
	for (auto it = hash_data_.constBegin(); it != hash_data_.constEnd(); ++it)
	{
		if (auto match = kRegexp.match(it.value());
			match.hasMatch()
			&& CommonTools::ValidParentheses(match.captured()
			                                      .toLocal8Bit()
			                                      .constData()))
		{
			hash_data_.insert(it.key(), GetValue(match.captured(1), it.key()));
		}
	}

	for (auto it = hash_response_.constBegin(); it != hash_response_.constEnd(); ++it)
	{
		if (auto match = kRegexp.match(it.value());
			match.hasMatch()
			&& CommonTools::ValidParentheses(match.captured()
			                                      .toLocal8Bit()
			                                      .constData()))
		{
			hash_response_.insert(it.key(), GetValue(match.captured(1), it.key()));
		}
	}
}

QHash<QString, QString> Controller::GetResponse()
{
	return hash_response_;
}

QString Controller::GetValue(QString arg_value, QString arg_key = "")
{
	auto list_string = arg_value.split('.');
	if (auto s = list_string.takeFirst(); !s.compare("Sql", Qt::CaseInsensitive))
	{
		if (auto v = list_string.takeFirst(); hash_sql_.contains(v))
		{
			auto [sql_name, sql_query, sql_result] = hash_sql_.value(v);
			if (auto vv = list_string.takeFirst(); !vv.compare("SqlResult"))
			{
				if (list_string.isEmpty())
				{
					if (sql_result.isEmpty())
					{
						// [1] 从数据库连接池里取得连接
						const QSqlDatabase db = ConnectionPoolSimple::OpenConnection(ConnectionPoolSimple::SqlServers().value(sql_name));

						// [2] 使用连接查询数据库
						QSqlQuery query(db);
						query.prepare(sql_query);
						for (auto it : kSqlRegexp.globalMatch(sql_query))
						{
							query.bindValue(it.captured(), GetValue(it.captured(1)));
						}
						query.setForwardOnly(true);
						query.exec();

						QJsonArray json_array_temp{};

						while (query.next())
						{
							QJsonObject temp_json_object{};
							for (int x{0}; x < query.record().count(); ++x)
							{
								temp_json_object.insert(query.record().fieldName(x), QJsonValue::fromVariant(query.value(x)));
							}
							json_array_temp.push_back(temp_json_object);
						}
						sql_result = QJsonDocument{json_array_temp}.toJson();
					}
					return sql_result;
				}
			}
			else
			{
				if (!vv.compare("SqlName", Qt::CaseInsensitive))
				{
					return sql_name;
				}
				if (!vv.compare("SqlQuery", Qt::CaseInsensitive))
				{
					return sql_query;
				}
			}
		}
	}
	else if (!s.compare("data", Qt::CaseInsensitive))
	{
		if (auto v = list_string.takeFirst(); list_string.isEmpty() && hash_data_.contains(v))
		{
			for (auto it : kRegexp.globalMatch(hash_data_.value(v)))
			{
				if (auto temp_string = GetValue(it.captured(1), v); temp_string.compare(it.captured(1)))
				{
					hash_data_.insert(v, temp_string);
				}
			}
			return hash_data_.value(v);
		}
	}
	else if (!s.compare("Parameters", Qt::CaseInsensitive))
	{
		if (const QString v = list_string.takeFirst(); list_string.isEmpty() && hash_parameters_.contains(v))
		{
			return hash_parameters_.value(v);
		}
	}
	else if (const QFile temp_file{s}; temp_file.exists())
	{
		if (const QMimeType temp_mimetype{QMimeDatabase{}.mimeTypeForFile(s)};
			temp_mimetype.name().startsWith("image/"))
		{
			return CommonTools::ConvertImgToBase64(s.toLocal8Bit());
		}
	}
	else if (!s.compare("GUID") || !s.compare("UUID"))
	{
		const QUuid uuid_uuid{QUuid::createUuid()};
		const QString string_base_data{"GUID"};
		return QUuid::createUuidV5(uuid_uuid, string_base_data).toString();
	}
	else if (!s.compare("CaptchaImage"))
	{
		std::default_random_engine random_engine_seed(static_cast<unsigned int>(time(nullptr)));
		std::uniform_int_distribution<> distribution_int(0, 32767);
		std::string string_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		std::string string_letter_code;
		string_letter_code.resize(6);
		for (char& i : string_letter_code)
		{
			i = string_chars[distribution_int(random_engine_seed) % string_chars.length()];
		}
		QByteArray bytearray_base64{CommonTools::ConvertMatToBase64(CommonTools::CreateCaptchaImage(string_chars))};
		if (hash_data_.contains(arg_key))
		{
			if (hash_data_.contains(arg_key + "code"))
			{
				hash_data_.insert(arg_key + "code", QString::fromStdString(string_letter_code));
			}
		}
		return bytearray_base64;
	}

	return arg_value;
}
