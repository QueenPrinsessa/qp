#pragma once
#include <cstdio>

namespace qp {
	extern FILE *	Sys_GetConsoleOut();
	extern void		Sys_FlushConsole();
	extern void		Sys_FreeConsole();
	extern void		Sys_OutputDebugString( const char * fmt, ... );
	extern bool		Sys_DebuggerPresent();
	extern void		Sys_DebugBreak();
	extern bool		Sys_CreateDirectory( const char * path );
	extern bool		Sys_InitializeConsole();
}