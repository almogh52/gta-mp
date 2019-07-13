#pragma once

#include <cstdint>

namespace gtamp {
	namespace hook {
		class address {
		public:
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