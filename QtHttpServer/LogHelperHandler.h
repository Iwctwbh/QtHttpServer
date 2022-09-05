#pragma once
#include <qdatetime.h>
#include <crow/logging.h>


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
