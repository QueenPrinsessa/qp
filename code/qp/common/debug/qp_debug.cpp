#include "engine.pch.h"
#include "qp_debug.h"
#include "common/math/qp_math.h"
#include "common/platform/windows/qp_windows.h"

#if defined( QP_ASSERTS_ENABLED )
namespace qpDebug {
	using atomicAssertLevel_t = atomic_t< assertLevel_t >;
#if defined( QP_DEBUG )
	inline atomicAssertLevel_t g_assertLevel = assertLevel_t::DEBUG;
#else
	inline atomicAssertLevel_t g_assertLevel = assertLevel_t::RELEASE;
#endif
	bool Assert( const assertLevel_t assertLevel, const char * assertMsg, const char * file, const int line, const char * function ) {
		if ( assertLevel < g_assertLevel ) {
			return true;
		}
		qpLog::Warning( "ASSERTION FAILED: %s (%d): %s: %s", file, line, function, assertMsg );
		Sys_DebugBreak();
		return false;
	}
	void SetAssertLevel( const assertLevel_t & assertLevel ) {
		g_assertLevel.store( assertLevel );
	}
}
#endif

void qpLog::PrintMessage ( FILE * stream, const char * prefix, const char * format, va_list args ) {
	char buffer[ 16384 ] {};
	size_t bufferSize = sizeof( buffer ) - 1;
	size_t bufferUsed = snprintf( buffer, sizeof( buffer ), "%s", prefix );
	if ( bufferUsed < bufferSize ) {
		bufferUsed += vsnprintf( buffer + bufferUsed, bufferSize - bufferUsed, format, args );
	}
	buffer[ qpMath::Min( bufferUsed, bufferSize ) ] = '\n';

	QP_DISCARD_RESULT fprintf( stream, "%s%s\n", prefix, buffer );
	OutputDebugStringA( buffer );
}
