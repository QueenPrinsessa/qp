#include "engine.pch.h"
#if defined( QP_D3D11 )

#include "qp_d3d11.h"

#include "common/containers/qp_array.h"
#include "common/string/qp_string.h"
#include "engine/debug/qp_log.h"

#include "qp/common/platform/windows/qp_windows.h"
#include "d3d11.h"
#include "wrl/client.h"
#include "dxgi1_5.h"

enum rasterizerState_t : uint32 {
	RS_BACK_FACE_CULLING,
	RS_COUNT
};
enum blendState_t : uint32 {
	BS_NONE,
	BS_COUNT
};
enum depthStencilState_t : uint32 {
	DS_READ_WRITE,
	DS_COUNT
};
enum samplerState_t : uint32 {
	SS_DEFAULT,
	SS_COUNT
};

struct d3d11Data_t {
	HWND windowHandle = NULL;
	Microsoft::WRL::ComPtr< IDXGISwapChain1 > swapChain;
	Microsoft::WRL::ComPtr< IDXGIFactory2 > dxgiFactory;
	Microsoft::WRL::ComPtr< ID3D11Device > device;
	Microsoft::WRL::ComPtr< ID3D11DeviceContext > deviceContext;
	Microsoft::WRL::ComPtr< ID3D11RenderTargetView > backBuffer;
	Microsoft::WRL::ComPtr< ID3D11DepthStencilView > depthBuffer;
	qpArray< Microsoft::WRL::ComPtr< ID3D11RasterizerState >, RS_COUNT > rasterizerStates;
	qpArray< Microsoft::WRL::ComPtr< ID3D11BlendState >, BS_COUNT > blendStates;
	qpArray< Microsoft::WRL::ComPtr< ID3D11DepthStencilState >, DS_COUNT > depthStencilStates;
	qpArray< Microsoft::WRL::ComPtr< ID3D11SamplerState >, SS_COUNT > samplerStates;
	bool supportsTearing = false;
};

bool qpD3D11::CreateSwapChain() {
	QP_ASSERT( m_data != NULL );
#if !defined( QP_RETAIL )
	const bool deviceDebug = true;
#else
	const bool deviceDebug = false;
#endif
	{
		UINT dxgiFactoryFlags = deviceDebug ? DXGI_CREATE_FACTORY_DEBUG : 0u;
		HRESULT result = CreateDXGIFactory2( dxgiFactoryFlags, IID_PPV_ARGS ( m_data->dxgiFactory.ReleaseAndGetAddressOf() ) );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create DXGIFactory." );
			return false;
		}
	}

	{
		BOOL supportsTearing = FALSE;
		Microsoft::WRL::ComPtr< IDXGIFactory5 > dxgiFactory5;
		if ( SUCCEEDED( m_data->dxgiFactory.As( &dxgiFactory5 ) ) ) {
			HRESULT result = dxgiFactory5->CheckFeatureSupport( DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supportsTearing, sizeof( supportsTearing ) );
			if ( SUCCEEDED( result ) ) {
				m_data->supportsTearing = static_cast< bool >( supportsTearing );
			} else {
				qpDebug::Info( "Tearing is unsupported." );
				m_data->supportsTearing = false;
			}
		}
	}

	Microsoft::WRL::ComPtr< IDXGIAdapter1 > adapter;
	{
		HRESULT result = m_data->dxgiFactory->EnumAdapters1( 0, &adapter );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to find an adapter." );
			return false;
		}
	} 
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		HRESULT result = adapter->GetDesc1( &adapterDesc );
		if ( SUCCEEDED( result ) ) {
			double dedicatedVRAM = static_cast< double >( adapterDesc.DedicatedVideoMemory );
			dedicatedVRAM /= 1000;
			dedicatedVRAM /= 1000;
			double dedicatedRAM = static_cast< double >( adapterDesc.DedicatedSystemMemory );
			dedicatedRAM /= 1000;
			dedicatedRAM /= 1000;
			double sharedRAM = static_cast< double >( adapterDesc.SharedSystemMemory );
			sharedRAM /= 1000;
			sharedRAM /= 1000;
			qpDebug::Info( "Adapter: %s", qpWideToUTF8( adapterDesc.Description ).c_str() );
			qpDebug::Info( "Dedicated Video Memory: %.3lf", dedicatedVRAM );
			qpDebug::Info( "Dedicated System Memory: %.3lf", dedicatedRAM );
			qpDebug::Info( "Shared System Memory: %.3lf", sharedRAM );
			qpDebug::Info( "Device ID: %u", adapterDesc.DeviceId );
			qpDebug::Info( "SubSysID: %u", adapterDesc.SubSysId );

		}
	}
	{
		const UINT creationFlags = deviceDebug ? D3D11_CREATE_DEVICE_DEBUG : 0u;
		const UINT numFeatureLevels = 2;
		D3D_FEATURE_LEVEL featureLevels[ numFeatureLevels ] {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		D3D_FEATURE_LEVEL selectedFeatureLevel;
		HRESULT result = D3D11CreateDevice( adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, creationFlags, &featureLevels[ 0 ], numFeatureLevels, D3D11_SDK_VERSION, &m_data->device, &selectedFeatureLevel, &m_data->deviceContext );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create device!" );
			return false;
		}
	}
	{
		const UINT swapChainFlags = m_data->supportsTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Flags = swapChainFlags;

		if ( m_data->supportsTearing ) {
			// we need to use flip model swapchain for tearing
			swapChainDesc.BufferCount = 2;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		}

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc {};
		swapChainFullscreenDesc.Windowed = TRUE;
		HRESULT result = m_data->dxgiFactory->CreateSwapChainForHwnd( m_data->device.Get(), m_data->windowHandle, &swapChainDesc, &swapChainFullscreenDesc, NULL, &m_data->swapChain );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create swap chain." );
			return false;
		}
	}
	{
		HRESULT result = m_data->dxgiFactory->MakeWindowAssociation( m_data->windowHandle, DXGI_MWA_NO_ALT_ENTER );
		if ( FAILED( result ) ) {
			qpDebug::Warning( "Failed to disable using alt+enter for fullscreen." );
		}
	}

	return true;
}

bool qpD3D11::CreateBackBuffer() {
	QP_ASSERT( m_data != NULL );
	Microsoft::WRL::ComPtr< ID3D11Texture2D > backBufferTexture;
	{
		HRESULT result = m_data->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), &backBufferTexture );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create back buffer texture." );
			return false;
		}
	}
	{
		HRESULT result = m_data->device.Get()->CreateRenderTargetView( backBufferTexture.Get(), NULL, m_data->backBuffer.GetAddressOf() );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create back buffer." );
			return false;
		}
	}
	return true;
}

bool qpD3D11::CreateDepthBuffer() {

	Microsoft::WRL::ComPtr< ID3D11Texture2D > depthBufferTexture;
	{
		RECT rect;
		if ( !GetClientRect( m_data->windowHandle, &rect ) ) {
			qpDebug::Error( "Failed to get width and height for depth buffer." );
			return false;
		}

		D3D11_TEXTURE2D_DESC depthBufferDesc {};
		depthBufferDesc.Width = qpVerifyStaticCast< UINT >( rect.right - rect.left );
		depthBufferDesc.Height = qpVerifyStaticCast< UINT >( rect.bottom - rect.top );
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		HRESULT result = m_data->device->CreateTexture2D( &depthBufferDesc, NULL, depthBufferTexture.GetAddressOf() );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create depth buffer texture." );
			return false;
		}
	}
	{
		HRESULT result = m_data->device->CreateDepthStencilView( depthBufferTexture.Get(), NULL, m_data->depthBuffer.GetAddressOf() );
		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create depth stencil view for depth buffer." );
			return false;
		}
	}
	return true;
}

static void SetupViewport( const int width, const int height, D3D11_VIEWPORT & outViewport ) {
	outViewport.MinDepth = 0.0f;
	outViewport.MaxDepth = 1.0f;
	outViewport.Width = static_cast< float >( width );
	outViewport.Height = static_cast< float >( height );
	outViewport.TopLeftX = 0.0f;
	outViewport.TopLeftY = 0.0f;
}

bool qpD3D11::SetViewportFromWindow() {
	RECT clientRect;
	if ( !GetClientRect( m_data->windowHandle, &clientRect ) ) {
		qpDebug::Error( "Failed to get window size for setting viewport" );
		return false;
	}
	const int numViewports = 1;
	D3D11_VIEWPORT viewports[ numViewports ];
	SetupViewport( clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, viewports[ 0 ] );
	m_data->deviceContext->RSSetViewports( numViewports, viewports );

	return true;
}

bool qpD3D11::CreateRasterizerStates() {
	{
		D3D11_RASTERIZER_DESC rasterizerStateDesc {};
		rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
		rasterizerStateDesc.FrontCounterClockwise = FALSE;
		rasterizerStateDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
		rasterizerStateDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerStateDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerStateDesc.DepthClipEnable = TRUE;
		rasterizerStateDesc.ScissorEnable = FALSE;
		rasterizerStateDesc.MultisampleEnable = FALSE;
		rasterizerStateDesc.AntialiasedLineEnable = FALSE;

		HRESULT result = m_data->device->CreateRasterizerState( &rasterizerStateDesc, &m_data->rasterizerStates[ RS_BACK_FACE_CULLING ] );

		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create back face culling rasterizer state." );
			return false;
		}
	}
	return true; 
}

bool qpD3D11::CreateBlendStates() {
	{
		m_data->blendStates[ BS_NONE ].Reset();
	}
	return true; 
}

bool qpD3D11::CreateDepthStencilStates() {
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		HRESULT result = m_data->device->CreateDepthStencilState( &depthStencilDesc, &m_data->depthStencilStates[ DS_READ_WRITE ] );

		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create read write depth stencil state." );
			return false;
		}
	}
	return true; 
}

bool qpD3D11::CreateSamplerStates() {
	{
		D3D11_SAMPLER_DESC samplerStateDesc {};
		samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerStateDesc.MipLODBias = 0.0f;
		samplerStateDesc.MaxAnisotropy = 1;
		samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerStateDesc.BorderColor[ 0 ] = 1.0f;
		samplerStateDesc.BorderColor[ 1 ] = 1.0f;
		samplerStateDesc.BorderColor[ 2 ] = 1.0f;
		samplerStateDesc.BorderColor[ 3 ] = 1.0f;
		samplerStateDesc.MinLOD = -D3D11_FLOAT32_MAX;
		samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT result = m_data->device->CreateSamplerState( &samplerStateDesc, m_data->samplerStates[ SS_DEFAULT ].GetAddressOf() );

		if ( FAILED( result ) ) {
			qpDebug::Error( "Failed to create default sampler state." );
			return false;
		}
	}
	return true; 
}

void qpD3D11::SetRasterizerState( const rasterizerState_t state ) {
	QP_ASSERT( state >= 0 && state < RS_COUNT );
	m_data->deviceContext->RSSetState( m_data->rasterizerStates[ state ].Get() );
}

void qpD3D11::SetBlendState( const blendState_t state, const qpVec4 & aBlendFactor, const uint32_t aSamplerMask ) {
	QP_ASSERT( state >= 0 && state < BS_COUNT );
	m_data->deviceContext->OMSetBlendState( m_data->blendStates[ state ].Get(), aBlendFactor.Data(), aSamplerMask );
}

void qpD3D11::SetBlendState ( const blendState_t state ) {
	SetBlendState( state, g_vec4Zero, 0xFFFFFFFF );
}

void qpD3D11::SetDepthStencilState( const depthStencilState_t state ) {
	QP_ASSERT( state >= 0 && state < DS_COUNT );
	m_data->deviceContext->OMSetDepthStencilState( m_data->depthStencilStates[ state ].Get(), 0 );
}

void qpD3D11::SetSamplerState( const uint32 slot, const samplerState_t state ) {
	QP_ASSERT( slot < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT );
	QP_ASSERT( state >= 0 && state < SS_COUNT );

	m_data->deviceContext->PSSetSamplers( slot, 1, m_data->samplerStates[ state ].GetAddressOf() );
	m_data->deviceContext->CSSetSamplers( slot, 1, m_data->samplerStates[ state ].GetAddressOf() );
}


void qpD3D11::Init ( void * windowHandle ) {
	QP_COMPILE_TIME_ASSERT( sizeof( HWND ) == sizeof( void * ) );
	m_data = new d3d11Data_t;
	m_data->windowHandle = static_cast< HWND >( windowHandle );

	qpDebug::Trace( "Creating swap chain." );
	if ( !CreateSwapChain() ) {
		qpDebug::Error( "Failed to create swap chain." );
		return;
	}
	qpDebug::Trace( "Swap chain created." );

	qpDebug::Trace( "Creating back buffer." );
	if ( !CreateBackBuffer() ) {
		qpDebug::Error( "Failed to create back buffer." );
		return;
	}
	qpDebug::Trace( "Back buffer created." );

	qpDebug::Trace( "Creating depth buffer!" );
	if ( !CreateDepthBuffer() ) {
		qpDebug::Error( "Failed to create depth buffer." );
		return;
	}
	qpDebug::Trace( "Depth buffer created." );

	qpDebug::Trace( "Creating rasterizer states." );
	if ( !CreateRasterizerStates() ) {
		qpDebug::Error( "Failed to create rasterizer states." );
		return;
	}
	qpDebug::Trace( "Rasterizer states created." );

	qpDebug::Trace( "Creating blend states." );
	if ( !CreateBlendStates() ) {
		qpDebug::Error( "Failed to create blend states." );
		return;
	}
	qpDebug::Trace( "Blend states created." );

	qpDebug::Trace( "Creating depth stencil states." );
	if ( !CreateDepthStencilStates() ) {
		qpDebug::Error( "Failed to create depth stencil states." );
		return;
	}
	qpDebug::Trace( "Depth stencil states created." );

	qpDebug::Trace( "Creating sampler states." );
	if ( !CreateSamplerStates() ) {
		qpDebug::Error( "Failed to create sampler states." );
		return;
	}
	qpDebug::Trace( "Sampler states created." );
}

void qpD3D11::DrawFrame () {
	// begin frame
	qpVec4 clearColor { 135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f };
	m_data->deviceContext.Get()->ClearRenderTargetView( m_data->backBuffer.Get(), clearColor.Data() );
	m_data->deviceContext.Get()->ClearDepthStencilView( m_data->depthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );

	SetBlendState( BS_NONE );
	SetDepthStencilState( DS_READ_WRITE );
	SetRasterizerState( RS_BACK_FACE_CULLING );
	SetSamplerState( 0u, SS_DEFAULT );

	SetViewportFromWindow();

	m_data->deviceContext.Get()->OMSetRenderTargets( 1, m_data->backBuffer.GetAddressOf(), m_data->depthBuffer.Get() );

	// draw stuff


	// end frame
	const bool useVsync = true;
	UINT syncInterval = useVsync ? 1 : 0;
	UINT presentFlags = ( !m_data->supportsTearing || useVsync ) ? 0 : DXGI_PRESENT_ALLOW_TEARING;
	m_data->swapChain->Present( syncInterval, presentFlags );
}

void qpD3D11::Cleanup () {
	delete m_data;
}

#endif