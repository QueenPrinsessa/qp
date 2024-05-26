#pragma once
#include "common/core/qp_types.h"
#include "common/string/qp_string.h"
#include "common/time/qp_time_point.h"
#include "common/utilities/qp_function.h"
#include "qp_thread_util.h"
#include <thread>

class qpThread {
public:
	using atomicThreadId_t = atomic_t< std::thread::id >;
	struct threadData_t {
		qpString threadName;
		atomicBool_t shouldTerminate = false;
		atomicBool_t isWorking = false;
		atomicBool_t isDetached = false;
		atomicThreadId_t id;
		QP_INTRUSIVE_REF_COUNTER;
	};
	using threadWorkFunctor_t = qpFunction< void( const threadData_t & threadData ) >;
	qpThread( const char * threadName );
	qpThread( const char * threadName, threadWorkFunctor_t && func );
	~qpThread();

	void RunJob( threadWorkFunctor_t && func );
	void RunJobDetached( threadWorkFunctor_t && func );
	bool WaitForThread( const qpTimePoint & timeout ); // returns false if timed out
	bool WaitForThreadInfinite(); // returns false if timed out
	void Join();
	void DetachThread();
	void Terminate();

	bool IsWorking() const;
	bool IsDetached() const;
	const char * GetName() const;

private:
	std::thread m_thread;
	qpIntrusiveRefPtr< threadData_t > m_threadData;

};
