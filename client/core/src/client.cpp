#include "../include/client.h"

#include <discord_rpc.h>
#include <subhook.h>

subhook::Hook h;

void gtamp::client::run()
{
	init_discord_rpc();

	while (true) {}
}

void gtamp::client::init_discord_rpc()
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