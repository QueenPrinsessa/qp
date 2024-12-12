#pragma once

namespace qp {
	class Job {
	public:
		virtual ~Job() = default;
		virtual void Run() = 0;
	};
}