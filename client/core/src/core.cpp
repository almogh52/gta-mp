#include "../include/core.h"

#include <discord_rpc.h>

class gtamp::core::core::core::impl
{
public:
	std::unordered_map<std::string, std::shared_ptr<manager_interface>> managers;
};

gtamp::core::core::core() : pimpl(new impl) {}

gtamp::core::core::~core() {}

void gtamp::core::core::init()
{
	// Init client logger
	log_manager::create_logger("Core");

	// Init discord RPC
	init_discord_rpc();
}

void gtamp::core::core::run()
{
	// Init managers
	spdlog::get("Core")->info("Initializing managers..");
	init_managers();
}

void gtamp::core::core::join_loop()
{
	while (true)
	{
	}
}

std::unordered_map<std::string, std::shared_ptr<gtamp::core::manager_interface>> &gtamp::core::core::managers()
{
	return pimpl->managers;
}

std::shared_ptr<gtamp::core::graphics::manager> gtamp::core::core::graphics_manager()
{
	return std::dynamic_pointer_cast<graphics::manager>(managers()["Graphics"]);
}

void gtamp::core::core::setup_logger(std::shared_ptr<spdlog::sinks::basic_file_sink_mt> sink)
{
	log_manager::set_sink(sink);
}

void gtamp::core::core::init_discord_rpc()
{
	DiscordEventHandlers handlers = {0};
	DiscordRichPresence discord_presence = {0};

	handlers.ready = nullptr;
	handlers.disconnected = nullptr;
	handlers.errored = nullptr;
	handlers.joinGame = nullptr;
	handlers.spectateGame = nullptr;
	handlers.joinRequest = nullptr;

	Discord_Initialize(DISCORD_APP_ID, &handlers, 1, NULL);

	discord_presence.state = "Developing";
	discord_presence.largeImageKey = "large";

	Discord_UpdatePresence(&discord_presence);
}

void gtamp::core::core::init_managers()
{
	// Alloc managers
	managers()["Graphics"] = std::make_shared<graphics::manager>(this);

	// For each manager, init it
	for (auto manager : managers())
	{
		manager.second->init();
	}
}