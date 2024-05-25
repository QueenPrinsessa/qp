#pragma once
#include "common/core/qp_types.h"
#include "qp/common/core/qp_sys_calls.h"
#include <cstdio>
#include <cstdarg>

#if !defined( QP_ASSERTS_ENABLED )
#if !defined( QP_RETAIL )
#define QP_ASSERTS_ENABLED
#endif
#endif

#define QP_COMPILE_TIME_ASSERT( expression ) static_assert( expression, #expression );
#define QP_COMPILE_TIME_ASSERT_MSG( expression, msg ) static_assert( expression, msg );

#if defined( QP_ASSERTS_ENABLED )
enum class assertLevel_t : uint8 {
	DEBUG,
	RELEASE
};
namespace qpDebug {
	extern bool Assert( const assertLevel_t assertLevel, const char * assertMsg, const char * file, const int line, const char * function );
	extern void SetAssertLevel( const assertLevel_t assertLevel );
}

#define QP_ASSERT_IMPL( expression, assertLevel, msg ) ( void )( ( expression ) || qpDebug::Assert( assertLevel_t::assertLevel, msg, __FILE__, __LINE__, __FUNCTION__ ) )
#define QP_VERIFY_IMPL( expression, assertLevel, msg ) ( ( expression ) || qpDebug::Assert( assertLevel_t::assertLevel, msg, __FILE__, __LINE__, __FUNCTION__ ) && false )
#else
#define QP_ASSERT_IMPL( expression, assertLevel, msg ) ( void )( 0 )
#define QP_VERIFY_IMPL( expression, assertLevel, msg ) ( expression )
#endif

// assert macros - expands to nothing when asserts are disabled
#define QP_ASSERT( expression ) QP_ASSERT_IMPL( ( expression ), DEBUG, "(" #expression ")" )
#define QP_ASSERT_RELEASE( expression ) QP_ASSERT_IMPL( ( expression ), RELEASE, "(" #expression ")" )
#define QP_ASSERT_MSG( expression, msg ) QP_ASSERT_IMPL( ( expression ), DEBUG, msg )
#define QP_ASSERT_RELEASE_MSG( expression, msg ) QP_ASSERT_IMPL( ( expression ), RELEASE, msg )
#define QP_ASSERT_ALWAYS( msg ) QP_ASSERT_IMPL( false, DEBUG, msg )

// verify macros - expands to just the expression when asserts are disabled
#define QP_VERIFY( expression ) QP_VERIFY_IMPL( ( expression ), DEBUG, "(" #expression ")" )
#define QP_VERIFY_RELEASE( expression ) QP_VERIFY_IMPL( ( expression ), RELEASE, "(" #expression ")" )
#define QP_VERIFY_MSG( expression, msg ) QP_VERIFY_IMPL( ( expression ), DEBUG, msg )
#define QP_VERIFY_RELEASE_MSG( expression, msg ) QP_VERIFY_IMPL( ( expression ), RELEASE, msg )

#pragma once

#define QP_CONSOLE_BLACK "\x1b[30m"
#define QP_CONSOLE_RED "\x1b[31m"
#define QP_CONSOLE_GREEN "\x1b[32m"
#define QP_CONSOLE_YELLOW "\x1b[33m"
#define QP_CONSOLE_BLUE "\x1b[34m"
#define QP_CONSOLE_MAGENTA "\x1b[35m"
#define QP_CONSOLE_CYAN "\x1b[36m"
#define QP_CONSOLE_WHITE "\x1b[37m"

#define QP_CONSOLE_BACKGROUND_BLACK "\x1b[40m"
#define QP_CONSOLE_BACKGROUND_RED "\x1b[41m"
#define QP_CONSOLE_BACKGROUND_GREEN "\x1b[42m"
#define QP_CONSOLE_BACKGROUND_YELLOW "\x1b[43m"
#define QP_CONSOLE_BACKGROUND_BLUE "\x1b[44m"
#define QP_CONSOLE_BACKGROUND_MAGENTA "\x1b[45m"
#define QP_CONSOLE_BACKGROUND_CYAN "\x1b[46m"
#define QP_CONSOLE_BACKGROUND_WHITE "\x1b[47m"

#define QP_CONSOLE_BRIGHT_BLACK "\x1b[90m"
#define QP_CONSOLE_BRIGHT_RED "\x1b[91m"
#define QP_CONSOLE_BRIGHT_GREEN "\x1b[92m"
#define QP_CONSOLE_BRIGHT_YELLOW "\x1b[93m"
#define QP_CONSOLE_BRIGHT_BLUE "\x1b[94m"
#define QP_CONSOLE_BRIGHT_MAGENTA "\x1b[95m"
#define QP_CONSOLE_BRIGHT_CYAN "\x1b[96m"
#define QP_CONSOLE_BRIGHT_WHITE "\x1b[97m"

#define QP_CONSOLE_BACKGROUND_BRIGHT_BLACK "\x1b[100m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_RED "\x1b[101m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_GREEN "\x1b[102m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_YELLOW "\x1b[103m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_BLUE "\x1b[104m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_MAGENTA "\x1b[105m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_CYAN "\x1b[106m"
#define QP_CONSOLE_BACKGROUND_BRIGHT_WHITE "\x1b[107m"

#define QP_CONSOLE_BRIGHT "\x1b[1m"
#define QP_CONSOLE_NO_BRIGHT "\x1b[22m"
#define QP_CONSOLE_UNDERLINE "\x1b[4m"
#define QP_CONSOLE_NO_UNDERLINE "\x1b[24m"
#define QP_CONSOLE_NEGATIVE "\x1b[7m"
#define QP_CONSOLE_POSITIVE "\x1b[27m"

#define QP_CONSOLE_DEFAULT_COLOR QP_CONSOLE_BACKGROUND_BLACK QP_CONSOLE_WHITE

namespace qpDebug {
	extern void PrintMessage( FILE * stream, const char * prefix, const char * format, va_list args );
	extern void PrintMessageEx( FILE * stream, const char * prefix, const char * color, const char * format, va_list args );
	static void Printf( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stdout, "", format, args);
		va_end( args );
	}
	static void Trace( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stdout, "TRACE: ", format, args);
		va_end( args );
	}
	static void Info( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessageEx( stdout, "INFO: ", QP_CONSOLE_GREEN, format, args );
		va_end( args );
	}
	static void Warning( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessageEx( stderr, "WARNING: ", QP_CONSOLE_BRIGHT_YELLOW, format, args );
		va_end( args );
	}

	static void Error( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessageEx( stderr, "ERROR: ", QP_CONSOLE_BACKGROUND_RED QP_CONSOLE_BRIGHT_WHITE, format, args );
		va_end( args );

		Sys_DebugBreak();
	}
};