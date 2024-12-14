#include "engine.pch.h"

#if defined( QP_PLATFORM_LINUX )
#include "qp/common/core/qp_sys_calls.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cctype>

namespace qp {
	FILE * Sys_GetConsoleOut () {
		QP_ASSERT_RELEASE_MSG( false, "Sys_GetConsoleOut is not implemented" );
		return NULL;
	}

	void Sys_FlushConsole () {
		QP_ASSERT_RELEASE_MSG( false, "Sys_FlushConsole is not implemented" );
	}

	void Sys_FreeConsole () {
		QP_ASSERT_RELEASE_MSG( false, "Sys_FreeConsole is not implemented" );
	}

	void Sys_OutputDebugString ( const char * fmt, ... ) {
		QP_ASSERT_RELEASE_MSG( false, "Sys_OutputDebugString is not implemented" );
	}

	bool Sys_DebuggerPresent() {
		QP_ASSERT_RELEASE_MSG( false, "Sys_DebuggerPresent is not implemented" );
#if defined( QP_RETAIL )
		return false;
#else
		// todo: linux: actually detect debugger, currently this will likely crash if you're not running one heh
		return true;
#endif
	}

	void Sys_DebugBreak() {
#if !defined( QP_RETAIL )
		if ( Sys_DebuggerPresent() ) {
			__builtin_debugtrap();
		}
#endif
	}

	bool Sys_CreateDirectory( const char * path ) {
		QP_ASSERT_RELEASE_MSG( false, "Sys_CreateDirectory is not implemented" );
		return true;
	}

	bool Sys_InitializeConsole() {
		// todo: linux: setup console
		QP_ASSERT_RELEASE_MSG( false, "Sys_InitializeConsole is not implemented" );
		return true;
	}
}
#endif
