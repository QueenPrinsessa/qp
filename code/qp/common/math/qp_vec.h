#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/math/qp_math.h"

// todo: implement vector intrinsics support
//#define QP_USE_VECTOR_INTRINSICS 
#define QP_ASSERT_NORMALIZE_ZERO_VECTOR

template < typename _type_, typename _vector_ >
class qpVecBase {
public:

	_type_ LengthSqr() const;
	_type_ Length() const;

	void Normalize() requires IsFloatingPoint< _type_ >;
	_vector_ Normalized() requires IsFloatingPoint< _type_ >;

	int NumElements() const { return QP_ARRAY_LENGTH( CRTP().m_data ); }

	_type_ & operator[]( int index ) { return CRTP().m_data[ index ]; }
	const _type_ & operator[]( int index ) const { return CRTP().m_data[ index ]; }

	_type_ * Data() { return CRTP().m_data; }
	const _type_ * Data() const { return CRTP().m_data; }

	void Zero();

	_vector_ operator-() const;

	_vector_ & CRTP() { return static_cast< _vector_ & >( *this ); }
	const _vector_ & CRTP() const { return static_cast< const _vector_ & >( *this ); }
};

template< typename _type_, typename _vector_ >
_type_ qpVecBase<_type_, _vector_>::LengthSqr() const {
	_type_ length = 0;
	for ( int index = 0; index < NumElements(); index++ ) {
		length += qpMath::Pow2( CRTP().m_data[ index ] );
	}
	return length;
}

template< typename _type_, typename _vector_ >
_type_ qpVecBase<_type_, _vector_>::Length() const {
	return qpMath::Sqrt( LengthSqr() );
}

template< typename _type_, typename _vector_ >
void qpVecBase< _type_, _vector_ >::Normalize() requires IsFloatingPoint< _type_ > {
	_type_ length = Length();
	if ( length == 0 ) {
#if defined( QP_ASSERT_NORMALIZE_ZERO_VECTOR )
		QP_ASSERT_MSG( length != 0, "Normalizing zero vector!" );
#endif
		return;
	}

	for ( int index = 0; index < NumElements(); index++ ) {
		CRTP().m_data[ index ] /= length;
	}
}

template< typename _type_, typename _vector_ >
_vector_ qpVecBase< _type_, _vector_ >::Normalized() requires IsFloatingPoint< _type_ > {
	_vector_ result = CRTP();
	result.Normalize();
	return result;
}

template< typename _type_, typename _vector_ >
void qpVecBase<_type_, _vector_>::Zero() {
	qpZeroMemory( &CRTP().m_data[ 0 ], sizeof( _type_ ) * NumElements() );
}

template< typename _type_, typename _vector_ >
_vector_ qpVecBase< _type_, _vector_ >::operator-() const {
	_vector_ result;
	for ( int index = 0; index < NumElements(); ++index ) {
		result[ index ] = -CRTP().m_data[ index ];
	}
	return result;
}

template< typename _type_, typename _vector_ >
inline _vector_ operator-=( qpVecBase< _type_, _vector_ > & lhs, const qpVecBase< _type_, _vector_ > & rhs ) {
	for ( int index = 0; index < lhs.NumElements(); ++index ) {
		lhs[ index ] -= rhs[ index ];
	}
	return lhs.CRTP();
}

template< typename _type_, typename _vector_ >
inline _vector_ operator-( const qpVecBase< _type_, _vector_ > & lhs, const qpVecBase< _type_, _vector_ > & rhs ) {
	_vector_ result( lhs.CRTP() );
	result -= rhs;
	return result;
}

template< typename _type_, typename _vector_ >
inline _vector_ operator+=( qpVecBase< _type_, _vector_ > & lhs, const qpVecBase< _type_, _vector_ > & rhs ) {
	for ( int index = 0; index < lhs.NumElements(); ++index ) {
		lhs[ index ] += rhs[ index ];
	}
	return lhs.CRTP();
}

template< typename _type_, typename _vector_ >
inline _vector_ operator+( const qpVecBase< _type_, _vector_ > & lhs, const qpVecBase< _type_, _vector_ > & rhs ) {
	_vector_ result( lhs.CRTP() );
	result += rhs;
	return result;
}

template< typename _type_, typename _vector_ >
inline _vector_ operator*=( qpVecBase< _type_, _vector_ > & lhs, const _type_ & rhs ) {
	for ( int index = 0; index < lhs.NumElements(); ++index ) {
		lhs[ index ] *= rhs;
	}
	return lhs.CRTP();
}

template< typename _type_, typename _vector_ >
inline _vector_ operator*( const qpVecBase< _type_, _vector_ > & lhs, const _type_ & rhs ) {
	_vector_ result( lhs.CRTP() );
	result *= rhs;
	return result;
}

template < typename _type_, int _length_ >
class qpVec : public qpVecBase< _type_, qpVec< _type_, _length_ > > {
public:
	union {
		_type_ m_data[ _length_ ] {};
		struct {
			_type_ x;
			_type_ y;
			_type_ z;
			_type_ w;
		};
	};
};

template < typename _type_ >
class qpVec< _type_, 2 > : public qpVecBase< _type_, qpVec< _type_, 2 > > {
public:
	qpVec()
		: qpVec( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ) ) {}
	qpVec( _type_ n )
		: qpVec( n, n ) {}
	qpVec( _type_ _x, _type_ _y )
		: m_data { _x, _y } {}
	explicit qpVec( const qpVec< _type_, 3 > & vec )
		: qpVec( vec.x, vec.y ) {}
	union {
		_type_ m_data[ 2 ] {};
		struct {
			_type_ x;
			_type_ y;
		};
	};
};

template < typename _type_ >
class qpVec< _type_, 3 > : public qpVecBase< _type_, qpVec< _type_, 3 > > {
public:
	qpVec()
		: qpVec( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ) ) {}
	qpVec( _type_ n )
		: qpVec( n, n, n ) {}
	qpVec( _type_ _x, _type_ _y, _type_ _z )
		: m_data { _x, _y, _z } {}
	explicit qpVec( const qpVec< _type_, 4 > & vec )
		: qpVec( vec.x, vec.y, vec.z ) {}
	explicit qpVec( const qpVec< _type_, 2 > & vec, const _type_ & _z )
		: qpVec( vec.x, vec.y, _z ) {}
	union {
		_type_ m_data[ 3 ] {};
		struct {
			_type_ x;
			_type_ y;
			_type_ z;
		};
	};
};

template < typename _type_ >
class qpVec< _type_, 4 > : public qpVecBase< _type_, qpVec< _type_, 4 > > {
public:
	qpVec()
		: qpVec( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ) ) {}
	qpVec( _type_ n )
		: qpVec( n, n, n, n ) {}
	qpVec( _type_ _x, _type_ _y, _type_ _z, _type_ _w )
		: m_data { _x, _y, _z, _w } {}
	explicit qpVec( const qpVec< _type_, 3 > & vec, const _type_ & _w )
		: qpVec( vec.x, vec.y, vec.z, _w ) {}
	union {
		_type_ m_data[ 4 ] {};
		struct {
			_type_ x;
			_type_ y;
			_type_ z;
			_type_ w;
		};
	};
};

//TODO: Add operators

template < typename _type_, typename _vector_ >
_type_ qpDot( const qpVecBase< _type_, _vector_ > & lhs, const qpVecBase< _type_, _vector_ > & rhs ) {
	_type_ dot = static_cast< _type_ >( 0 );
	for ( int index = 0; index < lhs.NumElements(); ++index ) {
		dot += lhs.CRTP().m_data[ index ] * rhs.CRTP().m_data[ index ];
	}
	return dot;
}

template < typename _type_ >
qpVec< _type_, 3 > qpCross( const qpVec< _type_, 3 > & lhs, const qpVec< _type_, 3 > & rhs ) {
	return qpVec< _type_, 3 >( ( ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ) ),
						  ( ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ) ),
						  ( ( lhs.x * rhs.y ) - ( lhs.y * rhs.x ) ) );
}

using qpVec2 = qpVec< float, 2 >;
using qpVec2d = qpVec< double, 2 >;
using qpVec2i = qpVec< int, 2 >;
using qpVec2ui = qpVec< unsigned int, 2 >;

using qpVec3 = qpVec< float, 3 >;
using qpVec3d = qpVec< double, 3 >;
using qpVec3i = qpVec< int, 3 >;
using qpVec3ui = qpVec< unsigned int, 3 >;

using qpVec4 = qpVec< float, 4 >;
using qpVec4d = qpVec< double, 4 >;
using qpVec4i = qpVec< int, 4 >;
using qpVec4ui = qpVec< unsigned int, 4 >;

static const qpVec2 g_vec2Zero { 0.0f };
static const qpVec3 g_vec3Zero { 0.0f };
static const qpVec4 g_vec4Zero { 0.0f };

static const qpVec3 g_vec3Right { 1.0f, 0.0f, 0.0f };
static const qpVec3 g_vec3Up { 0.0f, 1.0f, 0.0f };
static const qpVec3 g_vec3Forward { 0.0f, 0.0f, 1.0f };