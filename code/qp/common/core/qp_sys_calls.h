#pragma once
#include <cstdio>

extern FILE *	Sys_GetConsoleOut();
extern void		Sys_FlushConsole();
extern bool		Sys_DebuggerPresent();
extern void		Sys_DebugBreak();
extern bool		Sys_CreateDirectory( const char * path );
extern bool		Sys_InitializeConsole();