#pragma once

#include <core.h>

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

	static core::core& get_core()
	{
		return _core;
	};

private:
	inline static core::core _core;

	std::shared_ptr<spdlog::logger> _logger;

	pe_loader _loader;
};
}; // namespace launcher
}; // namespace gtamp