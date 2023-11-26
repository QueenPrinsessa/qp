#include "engine.pch.h"
#include "qp_debug.h"
#include "qp/common/debug/qp_debug.h"
#include <cstdio>
#include <iostream>

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/platform/windows/qp_windows.h"
#endif

bool qpDebug::InitializeConsole() {
#if defined( QP_PLATFORM_WINDOWS )
	FILE * consoleOut;
	AllocConsole();

	errno_t stdoutError = _wfreopen_s( &consoleOut, L"CONOUT$", L"w", stdout );
	QP_ASSERT_MSG( stdoutError == 0, "Failed to open stdout." );
	errno_t stderrError = _wfreopen_s( &consoleOut, L"CONOUT$", L"w", stderr );
	QP_ASSERT_MSG( stderrError == 0, "Failed to open stderr." );
	errno_t stdinError = _wfreopen_s( &consoleOut, L"CONOUT$", L"w", stdin );
	QP_ASSERT_MSG( stdinError == 0, "Failed to open stdin." );

	if( !consoleOut ) {
		return false;
	}

	if ( !( ( stdoutError == 0 ) && ( stderrError == 0 ) && ( stdinError == 0 ) ) ) {
		return false;
	}

	SetConsoleOutputCP( CP_UTF8 );
	setvbuf( consoleOut, nullptr, _IONBF, 1024 );

	CONSOLE_FONT_INFOEX consoleFontInfo;
	consoleFontInfo.cbSize = sizeof( consoleFontInfo );
	consoleFontInfo.nFont = 0;
	consoleFontInfo.dwFontSize.X = 10;
	consoleFontInfo.dwFontSize.Y = 20;
	consoleFontInfo.FontFamily = FF_DONTCARE;
	consoleFontInfo.FontWeight = FW_NORMAL;
	wcscpy_s( consoleFontInfo.FaceName, L"Consolas" );
	SetCurrentConsoleFontEx( GetStdHandle( STD_OUTPUT_HANDLE ), FALSE, &consoleFontInfo );

	return true;
#else
#error( "Unsupported platform." )
#endif
}

