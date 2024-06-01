#include "engine.pch.h"
#include "qp_thread_pool.h"
#include "qp/common/string/qp_string.h"
#include <condition_variable>

namespace {
	const uint32 s_minThreadPoolWorkers = 1;
	const milliseconds_t s_threadPoolShutdownTimeoutMs = milliseconds_t( 1000 );
}

qpThreadPool::qpThreadPool() {
}

qpThreadPool::qpThreadPool( const uint32 numWorkerThreads ) {
	Startup( numWorkerThreads );
}

qpThreadPool::~qpThreadPool () {
	QP_ASSERT_MSG( !m_started.load() && !m_shuttingDown.load(), "Thread pool should always be shutdown before being destroyed!");
}

void qpThreadPool::Startup( const uint32 numWorkerThreads ) {
	QP_ASSERT_MSG( !m_shuttingDown.load(), "Wait for thread pool to shutdown before starting it." );
	const uint32 numWorkersNeeded = qpMath::Clamp( numWorkerThreads, s_minThreadPoolWorkers, MaxWorkers() );
	qpDebug::Trace( "ThreadPool: Creating with %u workers.", numWorkersNeeded );
	m_threads.Reserve( numWorkersNeeded );
	qpString threadName;
	for ( uint32 index = 0; index < numWorkersNeeded; ++index ) {
		threadName.Format( "Worker %d", index );
		m_threads.Emplace( new qpThread( threadName.c_str(), QP_BIND_FUNCTION( qpThreadPool::DoWork ) ) );
	}

	m_started.store( true );
}

void qpThreadPool::Shutdown() {
	m_shuttingDown.store( true );

	qpDebug::Trace( "ThreadPool: Shutting down.");
	for( qpThread * thread : m_threads ) {
		qpDebug::Trace( "ThreadPool: Requesting to terminate thread '%s'.", thread->GetName() );
		thread->Terminate();
	}
	m_jobConditionVar.notify_all();

	for ( qpThread * thread : m_threads ) {
		if ( thread->WaitForThread( s_threadPoolShutdownTimeoutMs ) ) {
			qpDebug::Trace( "ThreadPool: Joining worker thread '%s'.", thread->GetName() );
			thread->Join();
		} else {
			thread->DetachThread();
			qpDebug::Warning( "ThreadPool: Failed to wait for thread '%s'. Detaching.", thread->GetName() );
		}
		delete thread;
	}
	m_threads.Clear();
	m_shuttingDown.store( false );
	m_started.store( false );
}

void qpThreadPool::QueueJob( threadJobFunctor_t && job ) {
	{
		std::scoped_lock lock( m_jobQueueMutex );
		m_jobsQueue.Emplace( qpMove( job ) );
	}
	m_jobConditionVar.notify_one();
}

void qpThreadPool::DoWork( const qpThread::threadData_t & threadData ) {
	threadJobFunctor_t job;
	while ( !threadData.shouldTerminate.load() ) {
		{
			std::unique_lock lock( m_jobQueueMutex );
			m_jobConditionVar.wait( lock, [ & ]() { return !m_jobsQueue.IsEmpty() || threadData.shouldTerminate.load(); } );
			if ( threadData.shouldTerminate.load() ) {
				break;
			}
			job = qpMove( m_jobsQueue.First() );
			m_jobsQueue.PopFirst();
		}
		job();
	}
}
