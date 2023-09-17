#ifdef QP_PLATFORM_WINDOWS

#include "qp/engine/platform/windows/window/qp_window_windows.h"

qpWindowsWindow::qpWindowsWindow( const qpWindowProperties_t & properties ) {
	Init( properties );
}

qpWindowsWindow::~qpWindowsWindow() {
	DestroyWindow( m_handle );
}

void qpWindowsWindow::OnUpdate() {

	MSG msg { };
	msg.hwnd = NULL;

	while ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	ShowWindow( m_handle, SW_SHOW );
	UpdateWindow( m_handle );
}

void qpWindowsWindow::Init( const qpWindowProperties_t & properties ) {
	const wchar_t * windowClassName = L"qpWindow";

	WNDCLASS windowClass {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.lpszClassName = windowClassName;
	RegisterClass( &windowClass );

	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE;

	if ( !properties.allowResize ) {
		windowStyle &= ~WS_THICKFRAME;
	}

	uint32_t primaryMonitorWidth = GetSystemMetrics( SM_CXSCREEN );
	uint32_t primaryMonitorHeight = GetSystemMetrics( SM_CYSCREEN );
	LONG windowLeft = static_cast< LONG >( ( primaryMonitorWidth - properties.width ) / 2 );
	LONG windowTop = static_cast< LONG >( ( primaryMonitorHeight - properties.height ) / 2 );

	m_width = properties.width;
	m_height = properties.height;

	HINSTANCE instanceHandle = NULL;

	if ( properties.platformData != NULL ) {
		qpWindowPropertiesWindows_t * windowsProperties = static_cast< qpWindowPropertiesWindows_t * >( properties.platformData );
		instanceHandle = windowsProperties->instanceHandle;
	}

	qpWString wTitle = qpUTF8ToWide( properties.title );
	m_handle = CreateWindow( windowClassName, wTitle.c_str(), windowStyle, windowLeft, windowTop, m_width, m_height, NULL, NULL, instanceHandle, this );

	SetForegroundWindow( m_handle );

	GetWindowPlacement( m_handle, &m_lastWindowPlacement );

	m_windowMode = properties.mode;
	ApplyWindowMode( m_windowMode );
}

void qpWindowsWindow::ApplyWindowMode( const qpWindowMode_t windowMode ) {
	DWORD windowStyle = GetWindowLong( m_handle, GWL_STYLE );

	switch ( windowMode ) {
		case qpWindowMode_t::WINDOWED:
		{
			SetWindowLong( m_handle, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW );
			SetWindowPlacement( m_handle, &m_lastWindowPlacement );

			RECT clientRect;
			GetClientRect( m_handle, &clientRect );
			int clientWidth = clientRect.right - clientRect.left;
			int clientHeight = clientRect.bottom - clientRect.top;
			SetWindowPos( m_handle, NULL, 0, 0, clientWidth, clientHeight,
						  SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
			SetForegroundWindow( m_handle );
			break;
		}
		case qpWindowMode_t::BORDERLESS:
		{
			MONITORINFO monitorInfo = { sizeof( monitorInfo ) };
			if ( GetWindowPlacement( m_handle, &m_lastWindowPlacement ) &&
				 GetMonitorInfo( MonitorFromWindow( m_handle, MONITOR_DEFAULTTOPRIMARY ), &monitorInfo ) ) {
				SetWindowLong( m_handle, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW );
				SetWindowPos( m_handle, HWND_TOP,
							  monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
							  monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
							  monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
							  SWP_NOOWNERZORDER | SWP_FRAMECHANGED );

				SetForegroundWindow( m_handle );
			}
			break;
		}
	}
}

LRESULT CALLBACK qpWindowsWindow::WndProc( _In_ HWND handle, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam ) {
	static qpWindowsWindow * window = NULL;

	switch ( msg ) {
		case WM_DESTROY:
		{
			DestroyWindow( handle );
			PostQuitMessage( 0 );
			break;
		}
		case WM_PAINT:
		{
			ValidateRect( handle, NULL );
			break;
		}
		case WM_CREATE:
		{
			const CREATESTRUCT * createdStruct = reinterpret_cast< CREATESTRUCT * >( lparam );
			window = static_cast< qpWindowsWindow * >( createdStruct->lpCreateParams );
			break;
		}
		default:
		{
			return DefWindowProc( handle, msg, wparam, lparam );
		}
	}

	return 0;
}

#endif
