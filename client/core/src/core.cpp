#include "../include/core.h"

#include <discord_rpc.h>

class gtamp::core::core::core::impl
{
public:
	impl() : event_manager(new gtamp::core::event_manager) {}

	std::unordered_map<std::string, std::shared_ptr<manager_interface>> managers;
	std::shared_ptr<gtamp::core::event_manager> event_manager;
};

gtamp::core::core::core() : pimpl(new impl) {}

gtamp::core::core::~core() {}

void gtamp::core::core::init()
{
	// Init client logger
	log_manager::create_logger("Core");

	// Init event manager
	event_manager()->init();

	// Init managers
	spdlog::get("Core")->info("Initializing managers..");
	init_managers();

	// Init discord RPC
	spdlog::get("Core")->info("Initializing Discord RPC..");
	init_discord_rpc();
}

void gtamp::core::core::join_loop()
{
	while (true)
	{
	}
}

std::shared_ptr<gtamp::core::event_manager> gtamp::core::core::event_manager()
{
	return pimpl->event_manager;
}

std::unordered_map<std::string, std::shared_ptr<gtamp::core::manager_interface>> &gtamp::core::core::managers()
{
	return pimpl->managers;
}

std::shared_ptr<gtamp::core::gfx::manager> gtamp::core::core::graphics_manager()
{
	return std::dynamic_pointer_cast<gfx::manager>(managers()["Graphics"]);
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
	managers()["Graphics"] = std::make_shared<gfx::manager>(this);

	// For each manager, init it
	for (auto manager : managers())
	{
		manager.second->init();
	}
}