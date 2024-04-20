#include "engine.pch.h"
#include <cstdio>

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/common/core/qp_sys_calls.h"
#include "qp/common/platform/windows/qp_windows.h"

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
	//int result = SHCreateDirectoryExA( NULL, path, NULL );
	//if ( result != ERROR_SUCCESS ) {
	//	const char * errStr = "";
	//	switch ( result ) {
	//		case ERROR_BAD_PATHNAME: { errStr = QP_STRINGIFY( ERROR_BAD_PATHNAME ); break; }
	//		case ERROR_FILENAME_EXCED_RANGE: { errStr = QP_STRINGIFY( ERROR_FILENAME_EXCED_RANGE ); break; }
	//		case ERROR_PATH_NOT_FOUND: { errStr = QP_STRINGIFY( ERROR_PATH_NOT_FOUND ); break; }
	//		case ERROR_FILE_EXISTS: { errStr = QP_STRINGIFY( ERROR_FILE_EXISTS ); break; }
	//		case ERROR_ALREADY_EXISTS: { errStr = QP_STRINGIFY( ERROR_ALREADY_EXISTS ); break; }
	//		case ERROR_CANCELLED: { errStr = QP_STRINGIFY( ERROR_CANCELLED ); break; }
	//	}
	//	qpLog::Error( "Failed to create directory with error: %s", errStr );
	//	return false;
	//}
	return true;
}

bool Sys_InitializeConsole() {
	FILE * consoleOut;
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

	SetConsoleOutputCP( CP_UTF8 );
	if ( setvbuf( consoleOut, NULL, _IONBF, 1024 ) == 0 ) {
		CONSOLE_FONT_INFOEX consoleFontInfo;
		consoleFontInfo.cbSize = sizeof( consoleFontInfo );
		consoleFontInfo.nFont = 0;
		consoleFontInfo.dwFontSize.X = 10;
		consoleFontInfo.dwFontSize.Y = 20;
		consoleFontInfo.FontFamily = FF_DONTCARE;
		consoleFontInfo.FontWeight = FW_NORMAL;
		wcscpy_s( consoleFontInfo.FaceName, L"Consolas" );
		SetCurrentConsoleFontEx( GetStdHandle( STD_OUTPUT_HANDLE ), FALSE, &consoleFontInfo );
	}

	return true;
}

#endif
