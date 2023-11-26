#pragma once
#include "qp/common/core/qp_macros.h"

// todo: implement vector intrinsics support
//#define QP_USE_VECTOR_INTRINSICS 
#define QP_ASSERT_NORMALIZE_ZERO_VECTOR

template < typename T, typename VEC >
class qpVecBase {
public:

	T LengthSqr() const;
	T Length() const;

	void Normalize() requires qpIsFloatingPoint< T >;
	int NumElements() const { return QP_ARRAY_LENGTH( CRTP().m_data ); }

	T & operator[]( int index ) { return CRTP().m_data[ index ]; }
	const T & operator[]( int index ) const { return CRTP().m_data[ index ]; }

	T * Data() const { return CRTP().m_data; }

	void Zero();
private:
	VEC & CRTP() { return static_cast< VEC & >( *this ); }
	const VEC & CRTP() const { return static_cast< const VEC & >( *this ); }
};

template< typename T, typename VEC >
T qpVecBase<T, VEC>::LengthSqr() const {
	T length = 0;
	for ( int index = 0; index < NumElements(); index++ ) {
		length += qpMath::Pow2( CRTP().m_data[ index ] );
	}
	return length;
}

template< typename T, typename VEC >
T qpVecBase<T, VEC>::Length() const {
	return qpMath::Sqrt( LengthSqr() );
}

template< typename T, typename VEC >
void qpVecBase<T, VEC>::Normalize() requires qpIsFloatingPoint< T > {
	T length = Length();
	if( length == 0 ) {
#if defined( QP_ASSERT_NORMALIZE_ZERO_VECTOR )
		QP_ASSERT_MSG( length != 0, "Normalizing zero vector!" );
#endif
		return;
	}

	for ( int index = 0; index < NumElements(); index++ ) {
		CRTP().m_data[ index ] /= length;
	}
}

template< typename T, typename VEC >
void qpVecBase<T, VEC>::Zero() {
	qpZeroMemory( &CRTP().m_data[ 0 ], sizeof(T) * NumElements());
}

template < typename T, int LENGTH >
class qpVec : public qpVecBase< T, qpVec< T, LENGTH > > {
public:
	union {
		T m_data[ LENGTH ] {};
		struct {
			T x;
			T y;
			T z;
			T w;
		};
	};
};

template < typename T >
class qpVec< T, 2 > : public qpVecBase< T, qpVec< T, 2 > > {
public:
	qpVec()
		: qpVec( static_cast< T >( 0 ), static_cast< T >( 0 ) ) { }
	qpVec( T n )
		: qpVec( n, n ) {}
	qpVec( T _x, T _y )
		: m_data { _x, _y } {}
	union {
		T m_data[ 2 ] {};
		struct {
			T x;
			T y;
		};
	};
};

template < typename T >
class qpVec< T, 3 > : public qpVecBase< T, qpVec< T, 3 > > {
public:
	qpVec()
		: qpVec( static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ) ) {}
	qpVec( T n )
		: qpVec( n, n, n ) {}
	qpVec( T _x, T _y, T _z )
		: m_data { _x, _y, _z } { }
	union {
		T m_data[ 3 ] {};
		struct {
			T x;
			T y;
			T z;
		};
	};
};

template < typename T >
class qpVec< T, 4 > : public qpVecBase< T, qpVec< T, 4 > > {
public:
	qpVec()
		: qpVec( static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ) ) { }
	qpVec( T n )
		: qpVec( n, n, n, n ) {}
	qpVec( T _x, T _y, T _z, T _w )
		: m_data { _x, _y, _z, _w } { }
	union {
		T m_data[ 4 ] {};
		struct {
			T x;
			T y;
			T z;
			T w;
		};
	};
};

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

#define QP_VEC2_ZERO qpVec2( 0.0f, 0.0f, 0.0f )
#define QP_VEC3_ZERO qpVec3( 0.0f, 0.0f, 0.0f )
#define QP_VEC4_ZERO qpVec4( 0.0f, 0.0f, 0.0f )