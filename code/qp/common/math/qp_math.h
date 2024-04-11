#pragma once
#include "qp/common/debug/qp_debug.h"
#include "qp/common/core/qp_type_traits.h"
#include <cmath>

namespace qpMath {
	constexpr static inline float Pi = 3.14159265359f;

	template <typename _type_>
	static _type_ Abs( const _type_ & n );

	template <typename _type_>
	static _type_ Min( const _type_ & a, const _type_ & b );

	template <typename _type_>
	static _type_ Max( const _type_ & a, const _type_ & b );

	template < typename _type_ >
	static _type_ Clamp( const _type_ & n, const _type_ & min, const _type_ & max );

	template < typename _type_ >
	static _type_ Clamp01( const _type_ & n );

	template < typename _type_ >
	static _type_ Deg2Rad( const _type_ & n );

	template < typename _type_ >
	static _type_ Rad2Deg( const _type_ & n );

	template < typename _type_ >
	static _type_ Cos( const _type_ & n );

	template < typename _type_ >
	static _type_ Sin( const _type_ & n );

	template < typename _type_ >
	static _type_ Asin( const _type_ & n );

	template < typename _type_ >
	static _type_ Tan( const _type_ & n );

	template < typename _type_ >
	static _type_ Atan( const _type_ & n );

	template < typename _type_ >
	static _type_ Atan2( const _type_ & y, const _type_ & x );

	template < typename _type_ >
	static _type_ Pow( const _type_ & a, const _type_ & b );

	template < typename _type_ >
	static _type_ Pow2( const _type_ & n );

	template < typename _type_ >
	static _type_ Sqrt( const _type_ & n );

	template < typename _type_ >
	static _type_ Ceil( const _type_ & n ) requires ( IsFloatingPoint< _type_ > );

	template < typename _type_ >
	static _type_ Log( const _type_ & n ) requires ( IsFloatingPoint< _type_ > );

	template < typename _type_ >
	static _type_ Log2( const _type_ & n ) requires ( IsFloatingPoint< _type_ > );

	template < typename _type_ >
	static _type_ RoundToPow2( const _type_ & n ) requires ( IsFloatingPoint< _type_ > );

	template < typename _type_, typename _time_ >
	static _type_ Lerp( const _type_ & a, const _type_ & b, const _time_ & time ) requires ( IsFloatingPoint< _time_ > );

	template < typename _type_ >
	static _type_ CopySign( const _type_ & magnitude, const _type_ & sign );
};

template < typename _type_ >
_type_ qpMath::Abs( const _type_ & n ) {
	return n >= 0 ? n : -n;
}

template < typename _type_ >
_type_ qpMath::Min( const _type_ & a, const _type_ & b ) {
	return ( a < b ) ? a : b;
}

template < typename _type_ >
_type_ qpMath::Max( const _type_ & a, const _type_ & b ) {
	return ( b < a ) ? a : b;
}

template < typename _type_ >
_type_ qpMath::Clamp( const _type_ & n, const _type_ & min, const _type_ & max ) {
	QP_ASSERT_MSG( min <= max, "Min can't be greater than max." );

	return ( n < min ) ? min : ( !( n < max ) ? max : n );
}

template < typename _type_ >
_type_ qpMath::Clamp01( const _type_ & n ) {
	return qpMath::Clamp( n, static_cast < _type_ >( 0 ), static_cast < _type_ >( 1 ) );
}

template < typename _type_ >
_type_ qpMath::Deg2Rad( const _type_ & n ) {
	return static_cast< _type_ >( n * ( Pi / 180.0f ) );
}

template < typename _type_ >
_type_ qpMath::Rad2Deg( const _type_ & n ) {
	return static_cast< _type_ > ( n * ( 180.0f / Pi ) );
}

template< typename _type_ >
_type_ qpMath::Cos( const _type_ & n ) {
	return static_cast< _type_ >( std::cos( n ) );
}

template< typename _type_ >
_type_ qpMath::Sin( const _type_ & n ) {
	return static_cast< _type_ >( std::sin( n ) );
}

template< typename _type_ >
_type_ qpMath::Asin( const _type_ & n ) {
	return static_cast< _type_ >( std::asin( n ) );
}

template< typename _type_ >
_type_ qpMath::Tan( const _type_ & n ) {
	return static_cast< _type_ >( std::tan( n ) );
}

template< typename _type_ >
_type_ qpMath::Atan( const _type_ & n ) {
	return static_cast< _type_ >( std::atan( n ) );
}

template< typename _type_ >
_type_ qpMath::Atan2( const _type_ & y, const _type_ & x ) {
	return std::atan2( y, x );
}

template< typename _type_ >
_type_ qpMath::Pow( const _type_ & a, const _type_ & b ) {
	return std::pow< _type_ >( a, b );
}

template< typename _type_ >
_type_ qpMath::Pow2( const _type_ & n ) {
	return n * n;
}

template< typename _type_ >
_type_ qpMath::Sqrt( const _type_ & n ) {
	return std::sqrt( n );
}

template< typename _type_ >
_type_ qpMath::Ceil( const _type_ & n ) requires ( IsFloatingPoint< _type_ > ) {
	return std::ceil( n );
}

template< typename _type_ >
_type_ qpMath::Log( const _type_ & n ) requires ( IsFloatingPoint< _type_ >) {
	return std::log( n );
}

template< typename _type_ >
_type_ qpMath::Log2( const _type_ & n ) requires ( IsFloatingPoint< _type_ > ) {
	return std::log2( n );
}

template < typename _type_ >
_type_ qpMath::RoundToPow2( const _type_ & n ) requires ( IsFloatingPoint< _type_ > ) {
	return qpMath::Pow( static_cast< _type_ >( 2 ), qpMath::Ceil( qpMath::Log( n ) / qpMath::Log( static_cast< _type_ >( 2 ) ) ) );
}

template< typename _type_, typename _time_ >
_type_ qpMath::Lerp( const _type_ & a, const _type_ & b, const _time_ & time ) requires ( IsFloatingPoint< _time_ > ) {
	return a + ( b - a ) * time;
}

template< typename _type_ >
_type_ qpMath::CopySign( const _type_ & magnitude, const _type_ & sign ) {
	return std::copysign( magnitude, sign );
}
