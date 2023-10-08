#pragma once
#include "qp/common/debug/qp_debug.h"
#include <cmath>

class qpMath {
public:

	constexpr static inline float Pi = 3.14159265359f;

	template <typename T>
	static T Abs( const T & n );

	template <typename T>
	static T Min( const T & a, const T & b );

	template <typename T>
	static T Max( const T & a, const T & b );

	template < typename T >
	static T Clamp( const T & n, const T & min, const T & max );

	template < typename T >
	static T Clamp01( const T & n );

	template < typename T >
	static T Deg2Rad( const T & n );

	template < typename T >
	static T Rad2Deg( const T & n );

	template < typename T >
	static T Pow( const T & a, const T & b );

	template < typename T >
	static T Ceil( const T & n ) requires ( std::is_floating_point_v< T > );

	template < typename T >
	static T Log( const T & n ) requires ( std::is_floating_point_v< T > );

	template < typename T >
	static T RoundToPow2( const T & n ) requires ( std::is_floating_point_v< T > );
private:
};

template < typename T >
T qpMath::Abs( const T & n ) {
	return n >= 0 ? n : -n;
}

template < typename T >
T qpMath::Min( const T & a, const T & b ) {
	return ( a < b ) ? a : b;
}

template < typename T >
T qpMath::Max( const T & a, const T & b ) {
	return ( b < a ) ? a : b;
}

template < typename T >
T qpMath::Clamp( const T & n, const T & min, const T & max ) {
	QP_ASSERT_MSG( min <= max, "Min can't be greater than max." );

	return ( n < min ) ? min : ( !( n < max ) ? max : n );
}

template < typename T >
T qpMath::Clamp01( const T & n ) {
	return qpMath::Clamp( n, static_cast < T >( 0 ), static_cast < T >( 1 ) );
}

template < typename T >
T qpMath::Deg2Rad( const T & n ) {
	return static_cast< T >( n * ( 180.0f / Pi ) );
}

template < typename T >
T qpMath::Rad2Deg( const T & n ) {
	return static_cast< T > ( n * ( Pi / 180.0f ) );
}

template< typename T >
T qpMath::Pow( const T & a, const T & b ) {
	return std::pow< T >( a, b );
}

template< typename T >
T qpMath::Ceil( const T & n ) requires ( std::is_floating_point_v<T> ) {
	return std::ceil( n );
}

template< typename T >
T qpMath::Log( const T & n ) requires (std::is_floating_point_v<T>) {
	return std::log( n );
}

template < typename T >
T qpMath::RoundToPow2( const T & n ) requires ( std::is_floating_point_v< T > ) {
	return qpMath::Pow( static_cast< T >( 2 ), qpMath::Ceil( qpMath::Log( n ) / qpMath::Log( static_cast< T >( 2 ) ) ) );
}
