#pragma once

#define QP_COMPARE_LESS_THAN( x, y ) ( ( x ) < ( y ) )
#define QP_COMPARE_GREATER_THAN( x, y ) ( QP_COMPARE_LESS_THAN( y, x ) )
#define QP_COMPARE_EQUALS( x, y ) ( !QP_COMPARE_LESS_THAN( x, y ) && !QP_COMPARE_LESS_THAN( y, x ) )
#define QP_COMPARE_LESS_THAN_EQUALS( x, y ) ( QP_COMPARE_LESS_THAN( x, y ) || QP_COMPARE_EQUALS( x, y ) )
#define QP_COMPARE_GREATER_THAN_EQUALS( x, y ) ( QP_COMPARE_LESS_THAN( y, x ) || QP_COMPARE_EQUALS( x, y ) )