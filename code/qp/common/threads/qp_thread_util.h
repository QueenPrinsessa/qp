#pragma once
#include <thread>

namespace  qp {
	class TimePoint;

	using threadId_t = std::thread::id;
	const threadId_t g_invalidThread = threadId_t();
	namespace thread_util {
		namespace internal {
			inline threadId_t g_mainThreadId = g_invalidThread;
		}

		static uint32 NumHardwareThreads() { return std::thread::hardware_concurrency(); }
		static threadId_t GetCurrentThreadId() { return std::this_thread::get_id(); }
		static bool IsCurrentThreadMainThread() { return internal::g_mainThreadId == GetCurrentThreadId(); }

		extern void SleepThread( const TimePoint & time );
		extern bool InitializeMainThread();
	}
}
