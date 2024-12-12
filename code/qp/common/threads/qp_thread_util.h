#pragma once
#include "common/time/qp_time_point.h"
#include <thread>

namespace  qp {
	namespace ThreadUtil {
		static uint32 NumHardwareThreads() { return std::thread::hardware_concurrency(); }
		static void SleepThread( const TimePoint & time ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( time.AsMilliseconds().Get() ) );
		}
	}
}
