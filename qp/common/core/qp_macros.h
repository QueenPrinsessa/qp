#pragma once

#define QP_UNUSED_PARAMETER( x ) x;
#define QP_STRINGIFY( x ) #x
#define QP_BIT( x ) ( 1u << ( x ) )

#define QP_NO_DISCARD [[ nodiscard ]]
#define QP_INLINE inline
#define QP_FORCE_INLINE __forceinline

#define QP_ITERATORS( type, beginImpl, endImpl ) \
		type Begin() { return beginImpl; } \
		type End() { return endImpl; } \
		type Begin() const { return beginImpl; } \
		type End() const { return endImpl; } \
		type begin() { return Begin(); } \
		type end() { return End(); } \
		type begin() const { return Begin(); } \
		type end() const { return End(); } 