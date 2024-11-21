#pragma once

class qpJob {
public:
	virtual ~qpJob() = default;
	virtual void Run() = 0;
};