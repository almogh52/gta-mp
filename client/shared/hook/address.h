#pragma once

#include <cstdint>

namespace gtamp {
	namespace hook {
		class address {
		public:
			address() : _addr(NULL) {};
			address(uintptr_t addr) : _addr(addr) {};

			template <typename T>
			operator T *()
			{
				return (T *)_addr;
			};

		protected:
			uintptr_t _addr;
		};
	};
};