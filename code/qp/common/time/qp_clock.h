#pragma once
#include "qp_time_point.h"

namespace qp {
	class Clock {
	public:
		static TimePoint Now();
		static timeTick_t TicksPerSecond();
	private:
	};
}