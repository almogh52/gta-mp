#pragma once

#include <string>
#include <vector>

#include <intrin.h>

namespace gtamp
{
namespace hook
{
class pattern
{
public:
	pattern(std::string pattern);

	uintptr_t get();
	std::vector<uintptr_t> get_matches();

private:
	std::vector<uint8_t> _mask;
	std::vector<uint8_t> _value;
	uint8_t _size;

	std::vector<uintptr_t> _matches;

	void search();

	void search_avx(__m256i &value, __m256i &mask, uintptr_t data);
	void search_sse(__m128i &value, __m128i &mask,  uintptr_t data);
	//void search_normal(JitRuntime& rt, void *data);

	void create_value(std::string pattern);
	void create_mask(std::string pattern);
	void pad(std::vector<uint8_t>& value, uint16_t amount);

	bool is_avx_supported();
	bool is_sse_supported();
};
}; // namespace hooking
}; // namespace gtamp