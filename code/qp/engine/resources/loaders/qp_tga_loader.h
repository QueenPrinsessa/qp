#pragma once
#include "qp_image_loader.h"

class qpTGALoader : public qpImageLoader {
protected:
	virtual const qpResource * LoadResource_Internal( const qpFile & file ) override;
private:
};
