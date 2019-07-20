#pragma once

#include "../manager_interface.h"

namespace gtamp
{
namespace core
{
namespace graphics
{
class manager : public manager_interface
{
public:
	manager(core *core);

	virtual void init();
};
}; // namespace graphics
}; // namespace core
}; // namespace gtamp