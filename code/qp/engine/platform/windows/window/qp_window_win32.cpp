#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/platform/windows/window/qp_window_win32.h"
#include "qp_mouse_win32.h"
#include "qp_keyboard_win32.h"
#include "qp/common/platform/windows/qp_windows.h"

namespace qp {
	struct windowWin32Data_t {
		windowWin32Data_t( HWND windowHandle ) : m_mouse( windowHandle ) {}
		Keyboard_Win32 m_keyboard;
		Mouse_Win32 m_mouse;
		WINDOWPLACEMENT m_lastWindowPlacement = { sizeof( WINDOWPLACEMENT ) };
	};

	Window_Win32::Window_Win32( const windowProperties_t & properties ) {
		Init( properties );
	}

	Window_Win32::~Window_Win32() {
		DestroyWindow( m_handle );
	}

	void Window_Win32::OnUpdate() {

		ShowWindow( m_handle, SW_SHOW );
		UpdateWindow( m_handle );

		MSG msg { };
		msg.hwnd = NULL;

		while ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		{
			RECT windowRect {};
			if ( GetWindowRect( m_handle, &windowRect ) ) {
				m_width = windowRect.right - windowRect.left;
				m_height = windowRect.bottom - windowRect.top;
			}
		}

		{
			RECT clientRect {};
			if ( GetClientRect( m_handle, &clientRect ) ) {
				m_clientWidth = clientRect.right - clientRect.left;
				m_clientHeight = clientRect.bottom - clientRect.top;
			}
		}

		m_data->m_mouse.Update();
		m_data->m_mouse.m_mouseCursor.Update();
		m_data->m_keyboard.Update();
	}

	const Mouse & Window_Win32::GetMouse() const {
		return m_data->m_mouse;
	}

	const Keyboard & Window_Win32::GetKeyboard() const {
		return m_data->m_keyboard;
	}

	void Window_Win32::Init( const windowProperties_t & properties ) {
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
		LONG windowLeft = ( primaryMonitorWidth - properties.width ) / 2;
		LONG windowTop = ( primaryMonitorHeight - properties.height ) / 2;

		m_width = properties.width;
		m_height = properties.height;

		HINSTANCE instanceHandle = NULL;

		if ( properties.platformData != NULL ) {
			windowPropertiesWindows_t * windowsProperties = static_cast< windowPropertiesWindows_t * >( properties.platformData );
			instanceHandle = windowsProperties->instanceHandle;
		}

		WideString wTitle = UTF8ToWide( properties.title );
		m_handle = CreateWindow( windowClassName, wTitle.c_str(), windowStyle, windowLeft, windowTop, m_width, m_height, NULL, NULL, instanceHandle, this );
		QP_ASSERT_RELEASE_MSG( m_data != NULL, "data should've been initialized in the create event" );

		SetForegroundWindow( m_handle );

		GetWindowPlacement( m_handle, &m_data->m_lastWindowPlacement );

		m_windowMode = properties.mode;
		ApplyWindowMode( m_windowMode );
	}

	void Window_Win32::ApplyWindowMode( const windowMode_t windowMode ) {
		DWORD windowStyle = GetWindowLong( m_handle, GWL_STYLE );

		switch ( windowMode ) {
			case windowMode_t::WINDOWED: {
				SetWindowLong( m_handle, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW );
				SetWindowPlacement( m_handle, &m_data->m_lastWindowPlacement );

				RECT clientRect;
				GetClientRect( m_handle, &clientRect );
				int clientWidth = clientRect.right - clientRect.left;
				int clientHeight = clientRect.bottom - clientRect.top;
				SetWindowPos( m_handle, NULL, 0, 0, clientWidth, clientHeight,
							  SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
				SetForegroundWindow( m_handle );
				break;
			}
			case windowMode_t::BORDERLESS: {
				MONITORINFO monitorInfo = { sizeof( monitorInfo ) };
				if ( GetWindowPlacement( m_handle, &m_data->m_lastWindowPlacement ) &&
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

	namespace {
		Window_Win32 * wndProcWindow = NULL;
	}
	LRESULT CALLBACK Window_Win32::WndProc( _In_ HWND handle, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam ) {

		if ( wndProcWindow ) {
			QP_ASSERT_RELEASE_MSG( wndProcWindow->m_data != NULL, "data should've been initialized in the create event" );
			if ( wndProcWindow->m_data->m_mouse.ProcessWindowEvent( msg, wparam, lparam ) ) {
				return 0;
			}

			if ( wndProcWindow->m_data->m_keyboard.ProcessWindowEvent( msg, wparam, lparam ) ) {
				return 0;
			}
		}

		switch ( msg ) {
			case WM_DESTROY: {
				DestroyWindow( handle );

				wndProcWindow->m_handle = NULL;
				if ( wndProcWindow->m_destroyCallback ) {
					wndProcWindow->m_destroyCallback();
				}

				PostQuitMessage( 0 );
				break;
			}
			case WM_SIZE: {
				UINT width = LOWORD( lparam );
				UINT height = HIWORD( lparam );
				if ( wndProcWindow->m_resizeCallback ) {
					wndProcWindow->m_resizeCallback( width, height );
				}
				break;
			}
			case WM_CREATE: {
				const CREATESTRUCT * createdStruct = reinterpret_cast< CREATESTRUCT * >( lparam );
				wndProcWindow = static_cast< Window_Win32 * >( createdStruct->lpCreateParams );
				wndProcWindow->m_data = CreateUnique< windowWin32Data_t >( handle );
				break;
			}
			default: {
				return DefWindowProc( handle, msg, wparam, lparam );
			}
		}

		return 0;
	}
}
#endif

#endif