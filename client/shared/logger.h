#pragma once

#ifdef GTAMPCORE_EXPORT
#define LOGGER_TRUNC false
#else
#define LOGGER_TRUNC true
#endif

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#define LOG_FILE "gtamp.log"

namespace gtamp
{
class log_manager
{
public:
	inline static std::shared_ptr<spdlog::logger> create_logger(std::string name)
	{
		auto logger = std::make_shared<spdlog::logger>(name, _file_sink);

		// Init the logger
		spdlog::initialize_logger(logger);

		return logger;
	};

	inline static void set_sink(std::shared_ptr<spdlog::sinks::basic_file_sink_mt> sink)
	{
		_file_sink = sink;
	};

	inline static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> get_sink()
	{
		return _file_sink;
	};

private:
	inline static auto _file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_FILE, LOGGER_TRUNC);
};
}; // namespace gtamp