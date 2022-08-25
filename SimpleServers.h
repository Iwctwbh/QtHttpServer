#pragma once
#ifndef SIMPLESERVERS_H
#define SIMPLESERVERS_H

#include <QtNetwork>
#include <crow.h>

class SimpleServers
{
public:
	SimpleServers();
	struct SimpleServer
	{
		//QByteArray *controller;
		QString method;
		QList<QByteArray> list_parameters;
		QByteArray bytearray_sql;

	};

	static constexpr std::vector<QString> GetVectorMethodStrings()
	{
		std::vector<QString> list_method_strings{};
		std::ranges::for_each(crow::method_strings, [&](const char* temp_ptr_char)
		{
			list_method_strings.emplace_back(temp_ptr_char);
		});
		return list_method_strings;
	}

	void InitSimpleServers();
	void InsertSimpleServer(const QByteArray&, const QByteArray&, const QList<QByteArray>&, const QByteArray&);
	static void EraseSimpleServer();
	void InitSimpleServersFromJson(const QJsonArray&);
	QMap<QByteArray, SimpleServer> GetSimpleServersMap();

private:
	QMap<QByteArray, SimpleServer> map_simple_servers_{};
};

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

#endif // SIMPLESERVERS_H
