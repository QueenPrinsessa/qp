#pragma once
#include "qp_types.h"

#define QP_UNUSED_PARAMETER( x ) x;
#define QP_STRINGIFY( x ) #x

#define QP_BIT( n ) ( 1u << ( n ) )
#define QP_GET_BIT( val, pos ) ( ( ( val ) >> static_cast< uint32 >( pos ) ) & 1u )
#define QP_SET_BIT( val, pos ) ( ( val ) |= ( 1u << static_cast< uint32 >( pos ) ) )
#define QP_CLEAR_BIT( val, pos ) ( ( val ) &= ~( 1u << static_cast< uint32 >( pos ) ) )
#define QP_TOGGLE_BIT( val, pos ) ( ( val ) ^= ( 1u << static_cast< uint32 >( pos ) ) )
#define QP_SET_BIT_TO( val, pos, b ) ( val = ( ( ( val ) & ~( 1u << static_cast< uint32 >( pos ) ) ) | ( static_cast< uint32 >( b ) << static_cast< uint32 >( pos ) ) ) )

#define QP_BIT_64( n ) ( 1ull << ( n ) )
#define QP_GET_BIT_64( val, pos ) ( ( ( val ) >> static_cast< uint64 >( pos ) ) & 1ull )
#define QP_SET_BIT_64( val, pos ) ( ( val ) |= ( 1ull << static_cast< uint64 >( pos ) ) )
#define QP_CLEAR_BIT_64( val, pos ) ( ( val ) &= ~( 1ull << static_cast< uint64 >( pos ) ) )
#define QP_TOGGLE_BIT_64( val, pos ) ( ( val ) ^= ( 1ull << static_cast< uint64 >( pos ) ) )
#define QP_SET_BIT_TO_64( val, pos, b ) ( val = ( ( ( val ) & ~( 1ull << static_cast< uint64 >( pos ) ) ) | ( static_cast< uint64 >( b ) << static_cast< uint64 >( pos ) ) ) )

#define QP_NO_DISCARD [[ nodiscard ]]
#define QP_INLINE inline
#define QP_FORCE_INLINE __forceinline

#define QP_ARRAY_LENGTH( x ) ( sizeof( x ) / sizeof( x[ 0 ] ) )

#define QP_SAFE_DELETE( x ) delete x; x = NULL;

#define QP_DISCARD( x ) static_cast< void >( x );
#define QP_DISCARD_RESULT ( void )

#define QP_FORWARD_ITERATOR( iteratorType, containerType, type ) \
	struct iteratorType { \
	public: \
		using iterator_category = std::forward_iterator_tag; \
		using difference_type = std::ptrdiff_t; \
		using value_type = type; \
		using pointer = type *; \
		using reference = type &; \
 \
		iteratorType( pointer ptr ) : m_ptr( ptr ) { } \
 \
		reference operator *() const { return *m_ptr; } \
 \
		pointer operator->() { return m_ptr; } \
		Iterator & operator++() { \
			m_ptr++; \
			return *this; \
		} \
		Iterator operator++( int ) { \
			Iterator it = m_ptr++; \
			return it; \
		} \
 \
		operator pointer () { return m_ptr; } \
 \
		auto operator<=>( const containerType::iteratorType & ) const = default; \
		bool operator==( const containerType::iteratorType & ) const = default; \
	private: \
		pointer m_ptr = NULL; \
	};

#define QP_ITERATORS( type, beginImpl, endImpl ) \
		type Begin() { return beginImpl; } \
		type End() { return endImpl; } \
		type Begin() const { return beginImpl; } \
		type End() const { return endImpl; } \
		type begin() { return Begin(); } \
		type end() { return End(); } \
		type begin() const { return Begin(); } \
		type end() const { return End(); } 