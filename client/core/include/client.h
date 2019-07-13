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
	client();
};
}; // namespace gtamp