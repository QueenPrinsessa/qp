#pragma once

#if defined( QP_D3D11 )
#include "qp_graphics_api.h"

class qpD3D11 : public qpGraphicsAPI {
public:
	virtual void Init ( void * windowHandle ) override;
	virtual void DrawFrame () override;
	virtual void Cleanup () override;

private:
};
#endif