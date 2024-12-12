#pragma once
#include "qp_ecs_job.h"

namespace qp {
	namespace game {
		class UpdateJob : public ECSJob {
		public:
			virtual void Run() override = 0;
		};
	}
}
