#pragma once
#include "qp_ecs_job.h"

class qpGameUpdateJob : public qpECSJob {
public:
	virtual void Run() override = 0;
};
