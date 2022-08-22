#include "SimpleServers.h"

SimpleServers::SimpleServers()
= default;

auto SimpleServers::InitSimpleServers() -> void
{
	this->map_simple_servers_.clear();
}

auto SimpleServers::InsertSimpleServer(const QByteArray& arg_bytearray_controller, const QList<QByteArray>& arg_list_parameters, const QByteArray& arg_bytearray_sql) -> void
{
	//QByteArray QByteArray_MD5_Temp = QCryptographicHash::hash(*QByteArray_controller_Temp, QCryptographicHash::Md5).toHex();
	const SimpleServers::SimpleServer simple_servers{ arg_list_parameters, arg_bytearray_sql };
	if (!this->map_simple_servers_.contains(arg_bytearray_controller))
	{
		this->map_simple_servers_.insert(arg_bytearray_controller, simple_servers);
	}
	else
	{
		qDebug() << L"不能有两个相同的控制器 Can't add two identical controllers";
	}
}

auto SimpleServers::EraseSimpleServer() -> void
{

}

auto SimpleServers::InitSimpleServersFromJson(const QJsonArray& arg_json_array) -> void
{
	this->InitSimpleServers();

	std::ranges::for_each(arg_json_array, [this](const QJsonValue& temp_json_value)
	{
		const QByteArray bytearray_controller{ temp_json_value.toObject().take("controller").toString().toLocal8Bit() };

		QList<QByteArray> list_parameters{};

		Q_FOREACH(const QString & temp_string, temp_json_value.toObject().take("Parameters").toString().split(','))
		{
			list_parameters.emplace_back(temp_string.trimmed().toLocal8Bit());
		}

		const QByteArray bytearray_sql{ temp_json_value.toObject().take("SQLString").toString().toLocal8Bit() };

		this->InsertSimpleServer(bytearray_controller, list_parameters, bytearray_sql);
	});
}
