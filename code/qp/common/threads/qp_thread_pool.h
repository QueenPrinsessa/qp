#pragma once
#include "qp_thread.h"
#include "common/containers/qp_list.h"

class qpThreadPool {
public:
	qpThreadPool();
	qpThreadPool( const uint32 numWorkerThreads );
	
	void Startup( const uint32 numWorkerThreads );
	void Shutdown();

	uint32 MaxWorkers() const { return m_threads.Length() != 0ull ? static_cast< uint32 >( m_threads.Length() ) : qpThreadUtil::NumHardwareThreads() - 1; }
private:
	qpList< qpThread * > m_threads;
	atomicBool_t m_started = false;
	atomicBool_t m_shuttingDown = false;
	void DoWork( const qpThread::threadData_t & threadData );
};
