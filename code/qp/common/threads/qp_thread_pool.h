#pragma once
#include "qp_thread.h"
#include "common/containers/qp_list.h"
#include "common/containers/qp_queue.h"
#include <condition_variable>
#include <mutex>

namespace qp {
	class ThreadPool {
	public:
		using threadJobFunctor_t = Function< void() >;
		ThreadPool();
		ThreadPool( const uint32 numWorkerThreads );
		~ThreadPool();

		void Startup( const uint32 numWorkerThreads );
		void Shutdown();

		uint32 MaxWorkers() const { return m_threads.Length() != 0ull ? static_cast< uint32 >( m_threads.Length() ) : thread_util::NumHardwareThreads() - 1; }

		void QueueJob( threadJobFunctor_t && job );

		void WaitForIdle();
	private:
		List< Thread * > m_threads;
		Queue< threadJobFunctor_t > m_jobsQueue;
		std::mutex m_jobQueueMutex;
		std::condition_variable m_jobConditionVar;
		uint32_t m_numTotalWorkers = 0;
		atomicUInt32_t m_numIdleWorkers = 0;
		atomicUInt32_t m_numActiveJobs = 0;
		atomicUInt32_t m_version = 0;
		atomicBool_t m_initialized = false;
		atomicBool_t m_shuttingDown = false;
		void DoWork( const Thread::threadData_t & threadData );
	};
}