#pragma once

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_window.h"
#include "qp/common/platform/windows/qp_types_win32.h"

namespace qp {
	struct windowPropertiesWindows_t {
		HINSTANCE instanceHandle = NULL;
	};

	struct windowWin32Data_t;
	class Window_Win32 : public Window {
	public:
		Window_Win32( const windowProperties_t & properties );
		virtual ~Window_Win32() override;

		virtual void OnUpdate() override;
		virtual void PollEvents() override;

		virtual int GetWidth() const override { return m_width; }
		virtual int GetHeight() const override { return m_height; }
		virtual int GetClientWidth() const override { return m_clientWidth; }
		virtual int GetClientHeight() const override { return m_clientHeight; }
		virtual void * GetHandle() const override { return m_handle; }
		virtual windowMode_t GetWindowMode() const override { return m_windowMode; }

		void SetWindowMode( const windowMode_t windowMode ) { m_windowMode = windowMode; }

		virtual const Mouse & GetMouse() const override;
		virtual const Keyboard & GetKeyboard() const override;
	private:
		void Init( const windowProperties_t & properties );
		void ApplyWindowMode( const windowMode_t windowMode );

		UniquePtr< windowWin32Data_t > m_data;
		HWND m_handle = NULL;
		int m_width = 1280;
		int m_height = 720;
		int m_clientWidth = 1280;
		int m_clientHeight = 720;
		windowMode_t m_windowMode = windowMode_t::WINDOWED;
		static LRESULT CALLBACK WndProc( _In_ HWND handle, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam );
	};
}
#endif

#endif