#pragma once

#ifdef GTAMPCORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif

#include <memory>

#include "../../shared/logger.h"

namespace gtamp
{
namespace client
{
class CORE_EXPORT client
{
public:
	void init();
	void run();
	void join_loop();

	void setup_logger(std::shared_ptr<spdlog::sinks::basic_file_sink_mt> sink);

private:
	inline static const char *DISCORD_APP_ID = "600342853511544843";

	void init_discord_rpc();
};
}; // namespace client
}; // namespace gtamp