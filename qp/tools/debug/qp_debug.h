#pragma once
#include <cassert>

#define QP_ASSERT( expression ) assert( (expression) )
#define QP_ASSERT_MSG( expression, msg ) assert( (expression ) && msg )