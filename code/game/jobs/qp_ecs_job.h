#pragma once
#include "qp/common/jobs/qp_job.h"
#include "QPEcs.hpp"

namespace qp {
	namespace game {
		class ECSJob : public Job {
		public:
			void Initialize( QPEcs::EntityComponentSystem & ecs ) {
				m_ecs = &ecs;
			}
		protected:
			QPEcs::EntityComponentSystem * m_ecs = NULL;
		};
	}
}
