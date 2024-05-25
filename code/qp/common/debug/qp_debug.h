#pragma once
#include "common/core/qp_macros.h"
#include <cassert>
#include "qp/common/core/qp_sys_calls.h"
#include <cstdarg>
#include <cstdio>
#include <mutex>

#if !defined( QP_ASSERTS_ENABLED )
#if !defined( QP_RETAIL )
#define QP_ASSERTS_ENABLED
#endif
#endif

#define QP_COMPILE_TIME_ASSERT( expression ) static_assert( expression, #expression );
#define QP_COMPILE_TIME_ASSERT_MSG( expression, msg ) static_assert( expression, msg );

#if defined( QP_ASSERTS_ENABLED )
namespace qpDebug {
	enum class assertLevel_t {
		DEBUG,
		RELEASE
	};
	extern bool Assert( const assertLevel_t assertLevel, const char * assertMsg, const char * file, const int line, const char * function );
	extern void SetAssertLevel( const assertLevel_t assertLevel );
}

#define QP_ASSERT_IMPL( expression, assertLevel, msg ) ( void )( ( expression ) || qpDebug::Assert( qpDebug::assertLevel_t::assertLevel, msg, __FILE__, __LINE__, __FUNCTION__ ) )
#define QP_VERIFY_IMPL( expression, assertLevel, msg ) ( ( expression ) || qpDebug::Assert( qpDebug::assertLevel_t::assertLevel, msg, __FILE__, __LINE__, __FUNCTION__ ) && false )
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

namespace qpDebug {
	extern void PrintMessage( FILE * stream, const char * prefix, const char * format, va_list args );
	static void Printf( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stdout, "", format, args);
		va_end( args );
	}
	static void Log( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stdout, "TRACE: ", format, args );
		va_end( args );
	}
	static void Info( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stdout, "INFO: ", format, args );
		va_end( args );
	}
	static void Warning( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stderr, "WARNING: ", format, args );
		va_end( args );
	}

	static void Error( const char * format, ... ) {
		va_list args;
		va_start( args, format );
		PrintMessage( stderr, "ERROR: ", format, args );
		va_end( args );

		Sys_DebugBreak();
	}
};