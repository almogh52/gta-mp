#pragma once

#include <memory>

#include "../../shared/logger.h"

namespace gtamp
{
namespace core
{
class core;

class manager_interface
{
public:
	manager_interface(std::string name, core *core)
		: _logger(log_manager::create_logger(name)), _core(core)
	{
		_logger->info("Initializing..");
	}

	virtual ~manager_interface() {}

	virtual void init() = 0;

protected:
	core *_core;
	std::shared_ptr<spdlog::logger> _logger;
};
}; // namespace core
}; // namespace gtamp