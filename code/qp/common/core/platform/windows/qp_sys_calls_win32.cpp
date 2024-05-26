#include "engine.pch.h"
#include "common/math/qp_math.h"
#include <cstdio>

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/core/qp_sys_calls.h"
#include "qp/common/platform/windows/qp_windows.h"

namespace {
	bool consoleInitialized = false;
	FILE * consoleOut = NULL;
}

FILE * Sys_GetConsoleOut() {
	return consoleOut;
}

void Sys_FlushConsole() {
	if ( consoleInitialized && consoleOut != NULL ) {
		QP_DISCARD_RESULT fflush( consoleOut );
	}
}

void Sys_FreeConsole () {
	if ( consoleInitialized ) {
		QP_DISCARD_RESULT fclose( consoleOut );
		consoleOut = NULL;
		FreeConsole();
		consoleInitialized = false;
	}
}

void Sys_OutputDebugString( const char * fmt, ... ) {
	char buffer[ 16384 ] {};
	va_list args;
	va_start( args, fmt );
	QP_DISCARD_RESULT vsnprintf( buffer, sizeof( buffer ), fmt, args );
	va_end( args );
	OutputDebugStringA( buffer );
}

bool Sys_DebuggerPresent() {
#if defined( QP_RETAIL )
	return false;
#else
	return IsDebuggerPresent();
#endif
}

void Sys_DebugBreak() {
#if !defined( QP_RETAIL )
	if ( Sys_DebuggerPresent() ) {
		__debugbreak();
	}
#endif
}

bool Sys_CreateDirectory( const char * path ) {
	( void )path;
	return true;
}

bool Sys_InitializeConsole() {
	AllocConsole();

	errno_t stdoutError = _wfreopen_s( &consoleOut, L"CONOUT$", L"w", stdout );
	QP_ASSERT_MSG( stdoutError == 0, "Failed to open stdout." );
	errno_t stderrError = _wfreopen_s( &consoleOut, L"CONOUT$", L"w", stderr );
	QP_ASSERT_MSG( stderrError == 0, "Failed to open stderr." );
	errno_t stdinError = _wfreopen_s( &consoleOut, L"CONOUT$", L"w", stdin );
	QP_ASSERT_MSG( stdinError == 0, "Failed to open stdin." );

	if ( !consoleOut ) {
		return false;
	}

	if ( !( ( stdoutError == 0 ) && ( stderrError == 0 ) && ( stdinError == 0 ) ) ) {
		return false;
	}

	HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	if ( consoleHandle == INVALID_HANDLE_VALUE ) {
		return false;
	}
	DWORD consoleMode = 0;
	if ( !GetConsoleMode( consoleHandle, &consoleMode ) ) {
		return false;
	}
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if ( !SetConsoleMode( consoleHandle, consoleMode ) ) {
		return false;
	}
	
	SetConsoleCtrlHandler( []( _In_ DWORD ctrlType ) -> BOOL {
			switch ( ctrlType ) {
				case CTRL_CLOSE_EVENT:
				case CTRL_SHUTDOWN_EVENT:
				case CTRL_LOGOFF_EVENT: {
					qpDebug::Trace( "Shutting down from console." );
					// if we don't flush the log file here we all our logs.
					// when you close the console window, since it terminates the process.
					qpDebug::FlushLogFile();
					return TRUE;
				}
			}
			return FALSE;
		}, TRUE );
	SetConsoleOutputCP( CP_UTF8 );
	QP_ASSERT( setvbuf( consoleOut, NULL, _IOLBF, 1024 ) == 0 );
	CONSOLE_FONT_INFOEX consoleFontInfo;
	consoleFontInfo.cbSize = sizeof( consoleFontInfo );
	consoleFontInfo.nFont = 0;
	consoleFontInfo.dwFontSize.X = 10;
	consoleFontInfo.dwFontSize.Y = 20;
	consoleFontInfo.FontFamily = FF_DONTCARE;
	consoleFontInfo.FontWeight = FW_NORMAL;
	wcscpy_s( consoleFontInfo.FaceName, L"Consolas" );
	SetCurrentConsoleFontEx( consoleHandle, FALSE, &consoleFontInfo );
	consoleInitialized = true;
	return true;
}

#endif
