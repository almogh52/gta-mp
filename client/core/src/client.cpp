#include "../include/client.h"

#include <discord_rpc.h>

#include "graphics/graphic_manager.h"

void gtamp::client::client::init()
{
	// Init client logger
	log_manager::create_logger("Client");

	init_discord_rpc();
}

void gtamp::client::client::run()
{
	graphic_manager graphic_manager;

	graphic_manager.init();
}

void gtamp::client::client::join_loop()
{
	while (true) {}
}

void gtamp::client::client::setup_logger(std::shared_ptr<spdlog::sinks::basic_file_sink_mt> sink)
{
	log_manager::set_sink(sink);
}

void gtamp::client::client::init_discord_rpc()
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