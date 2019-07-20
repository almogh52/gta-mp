#pragma once

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

private:
	std::shared_ptr<spdlog::logger> _logger;

	pe_loader _loader;
};
}; // namespace launcher
}; // namespace gtamp