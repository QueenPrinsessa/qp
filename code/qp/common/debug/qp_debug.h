#pragma once
#include <cassert>

#if !defined( QP_ASSERTS_ENABLED )
#if !defined( QP_RETAIL )
#define QP_ASSERTS_ENABLED
#endif
#endif

#define QP_COMPILE_TIME_ASSERT( expression ) static_assert( expression, #expression );
#define QP_COMPILE_TIME_ASSERT_MSG( expression, msg ) static_assert( expression, msg );

#if defined( QP_ASSERTS_ENABLED )
#define QP_ASSERT( expression ) assert( ( expression ) )
#define QP_ASSERT_MSG( expression, msg ) assert( ( expression ) && msg )

#define QP_VERIFY( expression ) ( expression )
#else
#define QP_ASSERT( expression ) ( ( void ) 0 )
#define QP_ASSERT_MSG( expression, msg ) ( ( void ) 0 )

#define QP_VERIFY( expression ) ( expression )
#endif