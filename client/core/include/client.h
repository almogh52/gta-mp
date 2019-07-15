#pragma once

#ifdef GTAMPCORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif

namespace gtamp
{
class CORE_EXPORT client
{
public:
	void run();

private:
	inline static const char *DISCORD_APP_ID = "600342853511544843";

	void init_discord_rpc();
};
}; // namespace gtamp