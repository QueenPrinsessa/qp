#pragma once
#include "qp/engine/input/qp_cursor.h"

#include "qp/common/platform/windows/qp_types_win32.h"
#include <mutex>

class qpMouseCursor_Win32 : public qpCursor {
	friend class qpMouse_Win32;
public:
	qpMouseCursor_Win32( HWND windowHandle );
	virtual void Update() override;
	
private:
	bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
	std::mutex m_pollMutex;
	HWND m_windowHandle = NULL;
	bool m_isFocused = false;
	// settings
	bool m_captureCursor = true;
	bool m_clipCursor = false;
	bool m_lockToCenter = true;
	// state
	bool m_capturedCursor = false;
	bool m_clippedCursor = false;
};
