#include "engine.pch.h"

#if defined( QP_PLATFORM_LINUX )
#include <cstdio>
#include "qp/common/core/qp_sys_calls.h"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cctype>

bool Sys_DebuggerPresent() {
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
	return true;
}

bool Sys_InitializeConsole() {
	// todo: linux: setup console
	return true;
}

#endif
