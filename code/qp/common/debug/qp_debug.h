#pragma once
#include <cassert>

#define QP_COMPILE_TIME_ASSERT( expression ) static_assert( expression, #expression );
#define QP_COMPILE_TIME_ASSERT_MSG( expression, msg ) static_assert( expression, msg );

#define QP_ASSERT( expression ) assert( (expression) )
#define QP_ASSERT_MSG( expression, msg ) assert( (expression ) && msg )