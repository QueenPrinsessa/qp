#pragma once
#include "qp/common/core/qp_sys_calls.h"
#include <iostream>

namespace qpLog {
	template< typename ... _args_ >
	static void Printf( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Log( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Info( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Warning( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Error( const char * format, _args_ &&... args );
};

template< typename ... _args_ >
void qpLog::Printf( const char * format, _args_ &&... args ) {
	printf( format, args... );
}

template< typename ... _args_ >
void qpLog::Log( const char * format, _args_ &&... args ) {
	printf( format, args... );
	printf( "\n" );
}

template< typename ... _args_ >
void qpLog::Info( const char * format, _args_ &&... args ) {
	printf( "INFO: " );
	printf( format, args... );
	printf( "\n" );
}

template< typename ... _args_ >
void qpLog::Warning( const char * format, _args_ &&... args ) {
	printf( "WARNING: " );
	printf( format, args... );
	printf( "\n" );
}

template< typename ... _args_ >
void qpLog::Error( const char * format, _args_ &&... args ) {
	printf( "ERROR: ");
	printf( format, args... );
	printf( "\n" );

	Sys_DebugBreak();
}
