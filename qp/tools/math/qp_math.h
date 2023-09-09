#pragma once
#include "qp/tools/debug/qp_debug.h"

class qpMath {
public:

	static inline float Pi = 3.14159265359f;

	template <typename T>
	static T Abs ( const T & n );

	template <typename T>
	static T Min ( const T & a, const T & b );

	template <typename T>
	static T Max ( const T & a, const T & b );

	template < typename T >
	static T Clamp ( const T & n, const T & min, const T & max );

	template < typename T >
	static T Clamp01 ( const T & n );

	template < typename T >
	static T Deg2Rad ( const T & n );

	template < typename T >
	static T Rad2Deg ( const T & n );
private:
};

template < typename T >
T qpMath::Abs ( const T & n ) {
	return n >= 0 ? n : -n;
}

template < typename T >
T qpMath::Min ( const T & a, const T & b ) {
	return ( a < b ) ? a : b;
}

template < typename T >
T qpMath::Max ( const T & a, const T & b ) {
	return ( b < a ) ? a : b;
}

template < typename T >
T qpMath::Clamp ( const T & n, const T & min, const T & max ) {
	qpAssertMsg ( min <= max, "Min can't be greater than max." );

	return ( n < min ) ? min : ( !( n < max ) ? max : n );
}

template < typename T >
T qpMath::Clamp01 ( const T & n ) {
	return qpMath::Clamp ( n, static_cast < T >( 0 ), static_cast < T >( 1 ) );
}

template < typename T >
T qpMath::Deg2Rad ( const T & n ) {
	return static_cast< T >( n * ( 180.0f / Pi ) );
}

template < typename T >
T qpMath::Rad2Deg ( const T & n ) {
	return static_cast< T > ( n * ( Pi / 180.0f ) );
}
