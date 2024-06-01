#pragma once
#include "qp_thread.h"
#include "common/containers/qp_list.h"
#include <mutex>

class qpThreadPool {
	using threadJobFunctor_t = qpFunction< void() >;
public:
	qpThreadPool();
	qpThreadPool( const uint32 numWorkerThreads );
	
	void Startup( const uint32 numWorkerThreads );
	void Shutdown();

	uint32 MaxWorkers() const { return m_threads.Length() != 0ull ? static_cast< uint32 >( m_threads.Length() ) : qpThreadUtil::NumHardwareThreads() - 1; }

	void QueueJob( threadJobFunctor_t && job );
private:
	qpList< qpThread * > m_threads;
	qpList< threadJobFunctor_t > m_jobsQueue;
	std::mutex m_jobQueueMutex;
	std::condition_variable m_jobConditionVar;
	atomicBool_t m_started = false;
	atomicBool_t m_shuttingDown = false;
	void DoWork( const qpThread::threadData_t & threadData );
};
