#pragma once

// todo: implement vector intrinsics support
//#define QP_USE_VECTOR_INTRINSICS 
#define QP_ASSERT_NORMALIZE_ZERO_VECTOR

template < typename T, typename VEC >
class qpVecBase {
public:

	T LengthSqr() const;
	T Length() const;

	void Normalize() requires qpIsFloatingPoint< T >;
	int NumElements() const { return sizeof( This().m_data ) / sizeof( T ); }

	T & operator[]( int index ) { return This().m_data[ index ]; }
	const T & operator[]( int index ) const { return This().m_data[ index ]; }

	T * Data() const { return This().m_data; }
private:
	VEC & This() { return static_cast< VEC & >( *this ); }
	const VEC & This() const { return static_cast< const VEC & >( *this ); }
};

template< typename T, typename VEC >
T qpVecBase<T, VEC>::LengthSqr() const {
	T length = 0;
	for ( int index = 0; index < NumElements(); index++ ) {
		length += qpMath::Pow2( This().m_data[ index ] );
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
#ifdef QP_ASSERT_NORMALIZE_ZERO_VECTOR
		QP_ASSERT_MSG( length != 0, "Normalizing zero vector!" );
#endif
		return;
	}

	for ( int index = 0; index < NumElements(); index++ ) {
		This().m_data[ index ] /= length;
	}
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
	qpVec() {}
	qpVec( T n )
		: qpVec( n, n ) {}
	qpVec( T _x, T _y )
		: x( _x ), y( _y ) {}
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
	qpVec() {}
	qpVec( T n )
		: qpVec( n, n, n ) {}
	qpVec( T _x, T _y, T _z )
		: x( _x ), y( _y ), z( _z ) { }
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
	qpVec() {}
	qpVec( T n )
		: qpVec( n, n, n, n ) {}
	qpVec( T _x, T _y, T _z, T _w )
		: x( _x ), y( _y ), z( _z ), w( _w ) { }
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