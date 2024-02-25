#pragma once
#include "qp_resource_loader.h"

class qpImageLoader : public qpResourceLoader {
protected:
	virtual const qpResource * LoadResource_Internal( const qpFile & file ) override;
private:
};
