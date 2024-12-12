#pragma once
#include "common/containers/qp_array.h"
#include "common/containers/qp_static_list.h"
#include "qp/common/threads/qp_thread_pool.h"

namespace qp {
	struct jobHandle_t {
		uint32 index;
		uint16 flags;
		uint16 version;
	};

	class JobSystem {
	public:
		enum : int16 {
			MAX_JOB_DEPENDENCIES = 4,
			MAX_JOBS = 1024
		};

		void LinkJobs( const jobHandle_t head, const jobHandle_t tail );
	private:
		struct job_t {
			ThreadPool::threadJobFunctor_t func;
			StaticList< jobHandle_t, MAX_JOB_DEPENDENCIES > dependencies;
			atomicBool_t finished = false;
		};
		struct storedJob_t {
			uint16 flags;
			uint16 version;
			uint32 next;
			job_t job;
		};
		Array< storedJob_t, MAX_JOBS > m_storedJobs;
		StaticList< jobHandle_t, MAX_JOBS > m_jobs;
	};
}