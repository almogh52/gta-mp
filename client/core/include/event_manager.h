#pragma once

#ifdef GTAMPCORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif

#include <functional>
#include <unordered_map>
#include <vector>

#include "event.h"

#include "../../shared/logger.h"

namespace gtamp
{
namespace core
{
class CORE_EXPORT event_manager
{
public:
	event_manager();

	void init();

	void on(event event, std::function<void()> handler);
	void call(event event);

private:
	class impl; impl *pimpl;
};
}; // namespace core
}; // namespace gtamp