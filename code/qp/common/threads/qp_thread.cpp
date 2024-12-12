#include "engine.pch.h"
#include "qp_thread.h"
#include "common/time/qp_clock.h"

namespace qp {
	Thread::Thread( const char * threadName ) {
		m_threadData = CreateIntrusiveRef< threadData_t >();
		m_threadData->threadName = threadName;
	}

	Thread::Thread( const char * threadName, threadWorkFunctor_t && func )
		: Thread( threadName ) {
		RunJob( Move( func ) );
	}

	Thread::~Thread() {
		QP_ASSERT_MSG( !m_thread.joinable(), "Threads must always be joined or detached before the thread is destroyed." );
	}

	void Thread::RunJob( threadWorkFunctor_t && func ) {
		QP_ASSERT_MSG( !m_thread.joinable() || !m_threadData->isWorking.load(), "There is already a job running on this thread. Wait for it to finish first." );
		m_threadData->isWorking.store( true );
		m_threadData->isDetached.store( false );
		m_thread = std::thread( [ job = Move( func ), threadDataRefPtr = m_threadData ]() mutable {
			threadData_t & threadData = *threadDataRefPtr;
			job( threadData );
			threadData.isWorking.store( false );
			debug::Trace( "Thread '%s' shutting down.", threadData.threadName.c_str() );
			} );
		m_threadData->id.store( m_thread.get_id() );
	}

	void Thread::RunJobDetached( threadWorkFunctor_t && func ) {
		RunJob( Move( func ) );
		DetachThread();
	}

	bool Thread::WaitForThread( const TimePoint & timeout ) {
		QP_ASSERT_MSG( m_threadData->id != std::this_thread::get_id(), "Deadlock. Threads should never wait on themselves." );
#if !defined( QP_RETAIL )
		if ( m_threadData->id == std::this_thread::get_id() ) {
			debug::CriticalError( "Thread attempted to wait on itself. ***NEEDS TO BE FIXED IMMEDIATELY***" );
			return false;
		}
#endif
		const TimePoint startWait = Clock::Now();
		while ( m_threadData->isWorking.load() ) {
			TimePoint timeWaited = Clock::Now() - startWait;
			if ( timeWaited > timeout ) {
				debug::Warning( "Wait for thread '%s' timed out after %lldms", m_threadData->threadName.c_str(), timeWaited.AsMilliseconds().Get() );
				return false;
			}
		}
		return true;
	}

	bool Thread::WaitForThreadInfinite() {
		return WaitForThread( g_timePointInfinity );
	}

	void Thread::Join() {
		QP_ASSERT_MSG( !m_threadData->isDetached.load(), "Thread should never be detached if need to join it." );
		if ( m_thread.joinable() ) {
			m_thread.join();
		}
	}

	void Thread::DetachThread() {
		QP_ASSERT_MSG( m_thread.joinable(), "We should never try to detach a thread that isn't joinable." );
		if ( m_thread.joinable() ) {
			m_thread.detach();
			m_threadData->isDetached.store( true );
		}
	}

	void Thread::Terminate() {
		m_threadData->shouldTerminate.store( true );
	}

	bool Thread::IsWorking() const {
		return m_threadData->isWorking.load();
	}

	bool Thread::IsDetached() const {
		return m_threadData->isDetached.load();
	}

	const char * Thread::GetName() const {
		return m_threadData->threadName.c_str();
	}
}