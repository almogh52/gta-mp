#pragma once

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#define LOG_FILE "gtamp.log"

namespace gtamp {
	inline static auto LOG_FILE_SINK = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_FILE, true);

	inline static std::shared_ptr<spdlog::logger> create_logger(std::string name)
	{
		auto logger = std::make_shared<spdlog::logger>(name, LOG_FILE_SINK);

		// Init the logger
		spdlog::initialize_logger(logger);

		return logger;
	};
}