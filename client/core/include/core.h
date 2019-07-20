#pragma once

#ifdef GTAMPCORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif

#include <string>
#include <memory>
#include <unordered_map>

#include "event_manager.h"
#include "../src/manager_interface.h"
#include "../src/graphics/manager.h"
#include "../../shared/logger.h"

namespace gtamp
{
namespace core
{
class CORE_EXPORT core
{
public:
	core();
	~core();

	void init();
	void join_loop();

	void setup_logger(std::shared_ptr<spdlog::sinks::basic_file_sink_mt> sink);

	std::shared_ptr<event_manager> event_manager();

	std::unordered_map<std::string, std::shared_ptr<manager_interface>> &managers();
	std::shared_ptr<graphics::manager> graphics_manager();

private:
	inline static const char *DISCORD_APP_ID = "600342853511544843";

	class impl; impl *pimpl;

	void init_managers();
	void init_discord_rpc();
};
}; // namespace client
}; // namespace gtamp