#include "../include/event_manager.h"

class gtamp::core::event_manager::impl {
public:
	std::unordered_map<event, std::vector<std::function<void()>>> handlers;
};

gtamp::core::event_manager::event_manager() : pimpl(new impl) {}

void gtamp::core::event_manager::init()
{
	log_manager::create_logger("Event Manager");
}

void gtamp::core::event_manager::on(gtamp::core::event event, std::function<void()> handler)
{
	// If the event has no handlers, create a new entry
	if (pimpl->handlers.find(event) == pimpl->handlers.end())
	{
		pimpl->handlers[event] = {handler};
	} else {
		pimpl->handlers[event].push_back(handler);
	}
}

void gtamp::core::event_manager::call(gtamp::core::event event)
{
	spdlog::get("Event Manager")->info("Event no. {} was called", event);

	// If the event has no handlers, ignore it
	if (pimpl->handlers.find(event) == pimpl->handlers.end())
	{
		return;
	}

	// Call each handler of the event
	for (auto handler : pimpl->handlers[event])
	{
		handler();
	}
}