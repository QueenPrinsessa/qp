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
	}

	bool Sys_DebuggerPresent() {
#if defined( QP_RETAIL )
		return false;
#else
		return false;
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
		return true;
	}
}
#endif
