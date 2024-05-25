#include "engine.pch.h"
#include "qp_debug.h"

namespace qpDebug {
#if defined( QP_ASSERTS_ENABLED )
	namespace {
		using atomicAssertLevel_t = atomic_t< assertLevel_t >;
#if defined( QP_DEBUG )
		static atomicAssertLevel_t g_assertLevel = assertLevel_t::DEBUG;
#else
		static atomicAssertLevel_t g_assertLevel = assertLevel_t::RELEASE;
#endif
	}

	bool Assert( const assertLevel_t assertLevel, const char * assertMsg, const char * file, const int line, const char * function ) {
		if ( assertLevel < g_assertLevel ) {
			return true;
		}
		if ( assertLevel >= assertLevel_t::RELEASE ) {
			Error( "ASSERTION FAILED: %s (%d): %s: %s", file, line, function, assertMsg );
		} else {
			Warning( "ASSERTION FAILED: %s (%d): %s: %s", file, line, function, assertMsg );
			Sys_DebugBreak(); // error already debugbreaks
		}
		return false;
	}
	void SetAssertLevel( const assertLevel_t assertLevel ) {
		g_assertLevel.store( assertLevel );
	}
#endif

	void PrintMessage( FILE * stream, const char * prefix, const char * format, va_list args ) {
		PrintMessageEx( stream, prefix, NULL, format, args);
	}

	void PrintMessageEx( FILE * stream, const char * prefix, const char * color, const char * format, va_list args ) {
		char buffer[ 16384 ] {};
		QP_DISCARD_RESULT vsnprintf( buffer, sizeof( buffer ), format, args );
		QP_DISCARD_RESULT fprintf( stream, "%s%s%s%s\n", color != NULL ? color : QP_CONSOLE_DEFAULT_COLOR, prefix, buffer, QP_CONSOLE_DEFAULT_COLOR );
		Sys_OutputDebugString( "%s%s\n", prefix, buffer );
	}
}
