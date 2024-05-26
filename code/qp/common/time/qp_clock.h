#pragma once
#include "qp_time_point.h"

class qpClock {
public:
	static qpTimePoint Now();
	static timeTick_t TicksPerSecond();
private:
};