#pragma once
#include "qp_thread.h"
#include "common/containers/qp_list.h"
#include "common/containers/qp_queue.h"
#include <mutex>

class qpThreadPool {
public:
	using threadJobFunctor_t = qpFunction< void() >;
	qpThreadPool();
	qpThreadPool( const uint32 numWorkerThreads );
	~qpThreadPool();
	
	void Startup( const uint32 numWorkerThreads );
	void Shutdown();

	uint32 MaxWorkers() const { return m_threads.Length() != 0ull ? static_cast< uint32 >( m_threads.Length() ) : qpThreadUtil::NumHardwareThreads() - 1; }

	void QueueJob( threadJobFunctor_t && job );

	void WaitForIdle();
private:
	qpList< qpThread * > m_threads;
	qpQueue< threadJobFunctor_t > m_jobsQueue;
	std::mutex m_jobQueueMutex;
	std::condition_variable m_jobConditionVar;
	std::condition_variable m_jobDoneConditionVar;
	uint32_t m_numTotalWorkers = 0;
	atomicUInt32_t m_numIdleWorkers = 0;
	atomicUInt32_t m_version = 0;
	atomicBool_t m_initialized = false;
	atomicBool_t m_shuttingDown = false;
	void DoWork( const qpThread::threadData_t & threadData );
};
