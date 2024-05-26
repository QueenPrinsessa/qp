#pragma once
#include "common/time/qp_time_point.h"
#include <thread>

namespace qpThreadUtil {
	static uint32 NumHardwareThreads() { return std::thread::hardware_concurrency(); }
	static void SleepThread( const qpTimePoint & time ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( time.AsMilliseconds().Get() ) );
	}
}
