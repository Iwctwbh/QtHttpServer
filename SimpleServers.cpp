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
	// Init Crow
	crow::logger::setHandler(&(this->handler_log_helper_));

	crow::App<SimpleServers::SimpleServerMiddleware> simple_app_crow{};
}
