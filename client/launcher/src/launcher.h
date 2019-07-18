#pragma once

#include <client.h>

#include "pe_loader.h"
#include "../../shared/logger.h"

namespace gtamp
{
namespace launcher
{
class launcher
{
public:
	launcher();

	void run();

	static client::client& get_client()
	{
		return _client;
	};

private:
	inline static client::client _client;

	std::shared_ptr<spdlog::logger> _logger;

	pe_loader _loader;
};
}; // namespace launcher
}; // namespace gtamp