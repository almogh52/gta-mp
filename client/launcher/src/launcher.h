#pragma once

#include "pe_loader.h"

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
	pe_loader _loader;
};
}; // namespace launcher
}; // namespace gtamp