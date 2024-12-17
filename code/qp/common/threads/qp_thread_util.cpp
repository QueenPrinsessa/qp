#include "engine.pch.h"

#include "qp_thread_util.h"
#include "qp/common/time/qp_time_point.h"

namespace qp {
	namespace thread_util {
		void SleepThread( const TimePoint & time ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( time.AsMilliseconds().Get() ) );
		}

		bool InitializeMainThread() {
			if ( QP_VERIFY_RELEASE_MSG( internal::g_mainThreadId == g_invalidThread, "main thread is already initialized" ) ) {
				internal::g_mainThreadId = GetCurrentThreadId();
				return true;
			}
			return false;
		}
	}
}