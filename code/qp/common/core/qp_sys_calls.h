#pragma once

extern bool Sys_DebuggerPresent();
extern void Sys_DebugBreak();
extern bool Sys_CreateDirectory( const char * path );
extern bool Sys_InitializeConsole();