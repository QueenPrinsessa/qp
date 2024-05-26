#include "engine.pch.h"
#include "qp_thread.h"
#include "common/time/qp_clock.h"

qpThread::qpThread( const char * threadName ) {
	m_threadData = qpCreateIntrusiveRef< threadData_t >();
	m_threadData->threadName = threadName;
}

qpThread::qpThread( const char * threadName, threadWorkFunctor_t && func )
	: qpThread( threadName ) {
	RunJob( qpMove( func ) );
}

qpThread::~qpThread(){
	QP_ASSERT_MSG( !m_thread.joinable(), "Threads must always be joined or detached before the thread is destroyed.");
}

void qpThread::RunJob( threadWorkFunctor_t && func ) {
	QP_ASSERT_MSG( !m_thread.joinable() || !m_threadData->isWorking.load(), "There is already a job running on this thread. Wait for it to finish first." );
	m_threadData->isWorking.store( true );
	m_threadData->isDetached.store( false );
	m_thread = std::thread( [ job = qpMove( func ), threadDataRefPtr = m_threadData, self = this ]() mutable {
			threadData_t & threadData = *threadDataRefPtr;
			job( threadData );
			threadData.isWorking.store( false );
			qpDebug::Trace( "Thread '%s' shutting down.", threadData.threadName.c_str() );
		} );
	m_threadData->id.store( m_thread.get_id() );
}

void qpThread::RunJobDetached( threadWorkFunctor_t && func ) {
	RunJob( qpMove( func ) );
	DetachThread();
}

bool qpThread::WaitForThread( const qpTimePoint & timeout ) {
	QP_ASSERT_MSG( m_threadData->id != std::this_thread::get_id(), "Deadlock. Threads should never wait on themselves." );
#if !defined( QP_RETAIL )
	if ( m_threadData->id == std::this_thread::get_id() ) {
		qpDebug::CriticalError( "Thread attempted to wait on itself. ***NEEDS TO BE FIXED IMMEDIATELY***" );
		return false;
	}
#endif
	const qpTimePoint startWait = qpClock::Now();
	while ( m_threadData->isWorking.load() ) {
		qpTimePoint timeWaited = qpClock::Now() - startWait;
		if ( timeWaited > timeout ) {
			qpDebug::Warning( "Wait for thread '%s' timed out after %lldms", m_threadData->threadName.c_str(), timeWaited.AsMilliseconds().Get() );
			return false;
		}
	}
	return true;
}

bool qpThread::WaitForThreadInfinite () {
	return WaitForThread( g_timePointInfinity );
}

void qpThread::Join() {
	QP_ASSERT_MSG( !m_threadData->isDetached.load(), "Thread should never be detached if need to join it." );
	if ( m_thread.joinable() ) {
		m_thread.join();
	}
}

void qpThread::DetachThread() {
	QP_ASSERT_MSG( m_thread.joinable(), "We should never try to detach a thread that isn't joinable.");
	if ( m_thread.joinable() ) {
		m_thread.detach();
		m_threadData->isDetached.store( true );
	}
}

void qpThread::Terminate(){
	m_threadData->shouldTerminate.store( true );
}

bool qpThread::IsWorking () const {
	return m_threadData->isWorking.load();
}

bool qpThread::IsDetached () const {
	return m_threadData->isDetached.load();
}

const char * qpThread::GetName () const {
	return m_threadData->threadName.c_str();
}
