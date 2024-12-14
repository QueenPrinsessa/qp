#include "engine.pch.h"
#include "qp_debug.h"
#include "common/math/qp_math.h"
#include "common/string/qp_string.h"
#include "common/time/qp_clock.h"
#include "common/threads/qp_thread_util.h"

namespace qp {
	namespace debug {
#if defined( QP_ASSERTS_ENABLED )
		namespace {
			using atomicAssertLevel_t = atomic_t< assertLevel_t >;
#if defined( QP_DEBUG )
			atomicAssertLevel_t s_assertLevel = assertLevel_t::DEBUG;
#else
			atomicAssertLevel_t s_assertLevel = assertLevel_t::RELEASE;
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
			using atomicDebugCategory_t = atomic_t< category_t >;
			atomicDebugCategory_t s_debugCategory = category_t::ALL;

			const TimePoint s_programStartTime = Clock::Now();
			struct logFileData_t {
				FILE * logFile = NULL;
				bool failedToOpen = false;
			} s_logFileData;

			constexpr const char * CategoryAsString( const category_t category ) {
#define CASE_RETURN_STRINGIFIED( x ) case x: return #x
				switch ( category ) {
					CASE_RETURN_STRINGIFIED( TRACE );
					CASE_RETURN_STRINGIFIED( INFO );
					CASE_RETURN_STRINGIFIED( WARNING );
					CASE_RETURN_STRINGIFIED( ERROR );
					CASE_RETURN_STRINGIFIED( CRITICAL ) " ERROR";
					case category_t::PRINT: return "";
				}
#undef CASE_RETURN_STRINGIFIED
				return "<UNKNOWN>";
			}
			size_t GetPrintPrefix( const category_t category, char * buffer, const size_t bufferSize ) {
				if ( category == category_t::PRINT ) {
					return 0;
				}
				const char * categoryStr = CategoryAsString( category );
				size_t length = strlen( categoryStr );
				if ( bufferSize < ( length + 3 ) ) {
					return 0;
				}
				strncpy( buffer, categoryStr, length );
				buffer[ length++ ] = ':';
				buffer[ length++ ] = ' ';
				buffer[ length ] = '\0';
				return length;
			}

			bool HasOpenedLogFile( const logFileData_t & logPrintData ) {
				return logPrintData.logFile != NULL && !logPrintData.failedToOpen;
			}
			void TryOpenLogFile( logFileData_t & logPrintData ) {
				const char * logFilePath = "console_log.txt";
				s_logFileData.failedToOpen = false;

				logPrintData.logFile = fopen( logFilePath, "w" );
				if ( logPrintData.logFile != NULL ) {
					const size_t logFileBufferSize = 1024ull * 1024ull;
					if ( setvbuf( logPrintData.logFile, NULL, _IOFBF, logFileBufferSize ) != 0 ) {
						QP_DISCARD_RESULT fclose( logPrintData.logFile );
						logPrintData.logFile = NULL;
						s_logFileData.failedToOpen = true;
					}
				} else {
					s_logFileData.failedToOpen = true;
				}
			}
		}

		void SetDebugCategories( const uint32 debugCategories ) {
			s_debugCategory.store( static_cast< debug::category_t >( debugCategories ) );
		}

		void FlushLogFile() {
			if ( HasOpenedLogFile( s_logFileData ) ) {
				QP_DISCARD_RESULT fflush( s_logFileData.logFile );
			}
		}

		void PrintMessage( const char * format, vaListParm_t & parm ) {
			PrintMessageEx( stdout, category_t::PRINT, NULL, format, parm );
		}

		void PrintMessageEx( FILE * stream, const category_t category, const char * color, const char * format, vaListParm_t & parm ) {
			if ( ( s_debugCategory.load() & category ) != category ) {
				return;
			}

			logFileData_t & logFileData = s_logFileData;
			if ( !HasOpenedLogFile( logFileData ) ) {
				TryOpenLogFile( logFileData );
			}
			char buffer[ 4096 ] {};
			size_t prefixLength = GetPrintPrefix( category, buffer, sizeof( buffer ) );
			const TimePoint timeSinceStart = Clock::Now() - s_programStartTime;
			const int timeSeconds = timeSinceStart.AsSeconds().GetI32();
			const int bufferPrintLength = vsnprintf( buffer + prefixLength, sizeof( buffer ) - prefixLength - 1, format, parm.args );
			if ( ( category & category_t::PRINT ) != category_t::PRINT ) {
				buffer[ math::Min( bufferPrintLength + prefixLength, sizeof( buffer ) - 1 ) ] = '\n';
			}
			QP_DISCARD_RESULT fprintf( stream, "[%d] %s%s%s", timeSeconds, color != NULL ? color : QP_CONSOLE_DEFAULT_COLOR, buffer, QP_CONSOLE_DEFAULT_COLOR );

			if ( HasOpenedLogFile( logFileData ) ) {
				QP_DISCARD_RESULT fprintf( logFileData.logFile, "[%d] %s", timeSeconds, buffer );
			}
			Sys_OutputDebugString( "[%d] %s", timeSeconds, buffer );
		}

		void CriticalError( const char * format, ... ) {
#if defined( QP_DEBUG_ERRORS )
			vaListParm_t parm;
			va_start( parm.args, format );
			PrintMessageEx( stderr, category_t::CRITICAL, QP_CONSOLE_BACKGROUND_RED QP_CONSOLE_BRIGHT_YELLOW, format, parm );
			va_end( parm.args );
#endif
			Sys_FlushConsole();
			ThreadUtil::SleepThread( milliseconds_t( 100 ) );
		}
	}
}