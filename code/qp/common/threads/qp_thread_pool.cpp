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
	QP_ASSERT_MSG( !m_initialized.load() && !m_shuttingDown.load(), "Thread pool should always be shutdown before being destroyed!");
}

void qpThreadPool::Startup( const uint32 numWorkerThreads ) {
	QP_ASSERT_MSG( !m_shuttingDown.load(), "Wait for thread pool to shutdown before starting it." );
	const uint32 numWorkersNeeded = qpMath::Clamp( numWorkerThreads, s_minThreadPoolWorkers, MaxWorkers() );
	qpDebug::Trace( "ThreadPool: Creating with %u workers.", numWorkersNeeded );
	m_threads.Reserve( numWorkersNeeded );
	m_numTotalWorkers = numWorkersNeeded;
	m_numIdleWorkers = numWorkersNeeded;

	++m_version;

	qpString threadName;
	for ( uint32 index = 0; index < numWorkersNeeded; ++index ) {
		threadName.Format( "Worker %d", index );
		m_threads.Emplace( new qpThread( threadName.c_str(), QP_BIND_FUNCTION( qpThreadPool::DoWork ) ) );
	}

	m_initialized.store( true );
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

	{
		std::scoped_lock lock( m_jobQueueMutex );
		m_jobsQueue.Clear();
	}

	// wake up any waiting threads in case any thread wasn't able to terminate.
	m_jobDoneConditionVar.notify_all();

	m_threads.Clear();
	m_numTotalWorkers = 0;
	m_numIdleWorkers = 0;

	m_shuttingDown.store( false );
	m_initialized.store( false );
}

void qpThreadPool::QueueJob( threadJobFunctor_t && job ) {
	QP_ASSERT_RELEASE_MSG( m_initialized.load(), "Job was queued before being started." );
	QP_ASSERT_RELEASE_MSG( !m_shuttingDown.load(), "Job was queued while shutting down." );
	{
		std::scoped_lock lock( m_jobQueueMutex );
		m_jobsQueue.Emplace( qpMove( job ) );
	}
	m_jobConditionVar.notify_one();
}

void qpThreadPool::WaitForIdle() {
	QP_ASSERT_RELEASE_MSG( m_initialized.load(), "Waiting for thread pool that hasn't been started." );
	std::unique_lock lock( m_jobQueueMutex );
	m_jobDoneConditionVar.wait( lock, [ & ] { 
		return m_jobsQueue.IsEmpty() && m_numIdleWorkers == m_numTotalWorkers || m_shuttingDown.load();
	} );
}

void qpThreadPool::DoWork( const qpThread::threadData_t & threadData ) {
	// cache which thread pool version we come from
	const uint32 threadPoolVersion = m_version.load();
	threadJobFunctor_t job;
	while ( !threadData.shouldTerminate.load() ) {
		{
			std::unique_lock lock( m_jobQueueMutex );
			m_jobConditionVar.wait( lock, [ & ]() { return !m_jobsQueue.IsEmpty() || threadData.shouldTerminate.load(); } );
			QP_ASSERT_RELEASE_MSG( threadPoolVersion == m_version.load(), "Thread pool should never have changed version while a thread was waiting for a job." );
			if ( threadData.shouldTerminate.load() ) {
				break;
			}
			--m_numIdleWorkers;
			if ( !QP_VERIFY_MSG( m_jobsQueue.Pop( job ), "Thread woke up to empty queue." ) ) {
				++m_numIdleWorkers;
				continue;
			}
		}
		job();
		if ( !m_initialized.load() || threadPoolVersion != m_version.load() ) {
			// thread pool was shutdown while we were in a job.
			QP_ASSERT_RELEASE_MSG( threadData.shouldTerminate.load(), "Threads should've been asked to terminate before the thread pool was shutdown." );
			break;
		}
		++m_numIdleWorkers;
		m_jobDoneConditionVar.notify_all();
	}
}
