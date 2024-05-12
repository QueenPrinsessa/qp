#pragma once

#if defined( QP_D3D11 )
#include "qp_graphics_api.h"
#include "common/math/qp_vec.h"

struct d3d11Data_t; // lives in cpp
enum rasterizerState_t : uint32;
enum blendState_t : uint32;
enum depthStencilState_t : uint32;
enum samplerState_t : uint32;

class qpD3D11 : public qpGraphicsAPI {
public:
	virtual void Init ( void * windowHandle ) override;
	virtual void DrawFrame () override;
	virtual void Cleanup () override;

private:
	d3d11Data_t * m_data = NULL;

	bool CreateSwapChain();
	bool CreateBackBuffer();
	bool CreateDepthBuffer();
	bool CreateRasterizerStates();
	bool CreateBlendStates();
	bool CreateDepthStencilStates();
	bool CreateSamplerStates();
	bool SetViewportFromWindow();
	void SetRasterizerState( const rasterizerState_t state );
	void SetBlendState( const blendState_t state, const qpVec4 & aBlendFactor, const uint32_t aSamplerMask );
	void SetBlendState( const blendState_t state );
	void SetDepthStencilState( const depthStencilState_t state );
	void SetSamplerState( const uint32 slot, const samplerState_t state );
};
#endif