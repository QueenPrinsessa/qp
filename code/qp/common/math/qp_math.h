#pragma once
#include "qp/common/debug/qp_debug.h"
#include <cmath>

namespace qpMath {
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
	static T Cos( const T & n );

	template < typename T >
	static T Sin( const T & n );

	template < typename T >
	static T Asin( const T & n );

	template < typename T >
	static T Tan( const T & n );

	template < typename T >
	static T Atan( const T & n );

	template < typename T >
	static T Atan2( const T & y, const T & x );

	template < typename T >
	static T Pow( const T & a, const T & b );

	template < typename T >
	static T Pow2( const T & n );

	template < typename T >
	static T Sqrt( const T & n );

	template < typename T >
	static T Ceil( const T & n ) requires ( std::is_floating_point_v< T > );

	template < typename T >
	static T Log( const T & n ) requires ( std::is_floating_point_v< T > );

	template < typename T >
	static T Log2( const T & n ) requires ( std::is_floating_point_v< T > );

	template < typename T >
	static T RoundToPow2( const T & n ) requires ( std::is_floating_point_v< T > );

	template < typename T, typename S >
	static T Lerp( const T & a, const T & b, const S & time ) requires ( std::is_floating_point_v< S > );

	template < typename T >
	static T CopySign( const T & magnitude, const T & sign );
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
	return static_cast< T >( n * ( Pi / 180.0f ) );
}

template < typename T >
T qpMath::Rad2Deg( const T & n ) {
	return static_cast< T > ( n * ( 180.0f / Pi ) );
}

template< typename T >
T qpMath::Cos( const T & n ) {
	return static_cast< T >( std::cos( n ) );
}

template< typename T >
T qpMath::Sin( const T & n ) {
	return static_cast< T >( std::sin( n ) );
}

template< typename T >
T qpMath::Asin( const T & n ) {
	return static_cast< T >( std::asin( n ) );
}

template< typename T >
T qpMath::Tan( const T & n ) {
	return static_cast< T >( std::tan( n ) );
}

template< typename T >
T qpMath::Atan( const T & n ) {
	return static_cast< T >( std::atan( n ) );
}

template< typename T >
T qpMath::Atan2( const T & y, const T & x ) {
	return std::atan2( y, x );
}

template< typename T >
T qpMath::Pow( const T & a, const T & b ) {
	return std::pow< T >( a, b );
}

template< typename T >
T qpMath::Pow2( const T & n ) {
	return n * n;
}

template< typename T >
T qpMath::Sqrt( const T & n ) {
	return std::sqrt( n );
}

template< typename T >
T qpMath::Ceil( const T & n ) requires ( std::is_floating_point_v<T> ) {
	return std::ceil( n );
}

template< typename T >
T qpMath::Log( const T & n ) requires (std::is_floating_point_v<T>) {
	return std::log( n );
}

template< typename T >
T qpMath::Log2( const T & n ) requires ( std::is_floating_point_v<T> ) {
	return std::log2( n );
}

template < typename T >
T qpMath::RoundToPow2( const T & n ) requires ( std::is_floating_point_v< T > ) {
	return qpMath::Pow( static_cast< T >( 2 ), qpMath::Ceil( qpMath::Log( n ) / qpMath::Log( static_cast< T >( 2 ) ) ) );
}

template< typename T, typename S >
T qpMath::Lerp( const T & a, const T & b, const S & time ) requires ( std::is_floating_point_v< S > ) {
	return a + ( b - a ) * time;
}

template< typename T >
T qpMath::CopySign( const T & magnitude, const T & sign ) {
	return std::copysign( magnitude, sign );
}
