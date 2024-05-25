#include "engine.pch.h"
#include "qp_debug.h"
#include "common/time/qp_clock.h"

namespace qpDebug {
#if defined( QP_ASSERTS_ENABLED )
	namespace {
		using atomicAssertLevel_t = atomic_t< assertLevel_t >;
#if defined( QP_DEBUG )
		atomicAssertLevel_t s_assertLevel = assertLevel_t::DEBUG;
#else
		atomicAssertLevel_t g_assertLevel = assertLevel_t::RELEASE;
#endif
	}

	bool Assert( const assertLevel_t assertLevel, const char * assertMsg, const char * file, const int line, const char * function ) {
		if ( assertLevel < s_assertLevel ) {
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
		s_assertLevel.store( assertLevel );
	}
#endif

	namespace {
		const qpTimePoint s_programStartTime = qpClock::Now();

		constexpr const char * CategoryAsString( const category_t category ) {
#define CASE_RETURN_STRINGIFIED( x ) case x: return #x
			switch ( category ) {
				CASE_RETURN_STRINGIFIED( TRACE );
				CASE_RETURN_STRINGIFIED( INFO );
				CASE_RETURN_STRINGIFIED( WARNING );
				CASE_RETURN_STRINGIFIED( ERROR );
				case category_t::PRINT: return "";
			}
#undef CASE_RETURN_STRINGIFIED
			return "";
		}
	}
	void PrintMessage( FILE * stream, const category_t category, const char * format, va_list args ) {
		PrintMessageEx( stream, category, NULL, format, args);
	}

	void PrintMessageEx( FILE * stream, const category_t category, const char * color, const char * format, va_list args ) {
		const char * categoryStr = CategoryAsString( category );
		char buffer[ 16384 ] {};
		const qpTimePoint timeSinceStart = qpClock::Now() - s_programStartTime;
		const int timeSeconds = static_cast< int >( timeSinceStart.AsSeconds() );
		QP_DISCARD_RESULT vsnprintf( buffer, sizeof( buffer ), format, args );
		QP_DISCARD_RESULT fprintf( stream, "[%d] %s%s: %s%s\n", timeSeconds, color != NULL ? color : QP_CONSOLE_DEFAULT_COLOR, categoryStr, buffer, QP_CONSOLE_DEFAULT_COLOR );
		Sys_OutputDebugString( "[%d] %s: %s\n", timeSeconds, categoryStr, buffer );
	}
}
