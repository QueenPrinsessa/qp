#pragma once
#include "qp_vec.h"

template < typename T, typename MAT, int SIZE >
class qpMatBase {
public:
	int NumElements() const { return SIZE * SIZE; }

	qpVec< T, SIZE > & operator[]( int rowIndex );
	const qpVec< T, SIZE > & operator[]( int rowIndex ) const;

	T & operator()( int row, int column );
	const T & operator()( int row, int column ) const;

	auto operator<=>( const qpMatBase & other ) const { return memcmp( CRTP().m_data, other.CRTP().m_data, SIZE * SIZE * sizeof( T ) ); }
	bool operator==( const qpMatBase & other ) const { return ( memcmp( CRTP().m_data, other.CRTP().m_data, SIZE * SIZE * sizeof( T ) ) == 0 ); }

	qpMatBase & operator+=( const qpMatBase & rhs );
	qpMatBase & operator-=( const qpMatBase & rhs );
	qpMatBase & operator*=( const qpMatBase & rhs );
	qpMatBase & operator*=( const float rhs );

	qpMatBase & Transpose();

	qpVec< T, 3 > Forward() const { return static_cast< qpVec< T, 3 > >( CRTP().m_vectors[ 0 ] ); }
	qpVec< T, 3 > Right() const { return static_cast< qpVec< T, 3 > >( CRTP().m_vectors[ 1 ] ); }
	qpVec< T, 3 > Up() const { return static_cast< qpVec< T, 3 > >( CRTP().m_vectors[ 2 ] ); }

	T * Data() const { return CRTP().m_data; }
private:
	MAT & CRTP() { return static_cast< MAT & >( *this ); }
	const MAT & CRTP() const { return static_cast< const MAT & >( *this ); }
};

template< typename T, typename MAT, int SIZE >
qpVec< T, SIZE > & qpMatBase< T, MAT, SIZE >::operator[]( int rowIndex ) {
	QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < SIZE ) );
	return CRTP().m_vectors[ rowIndex ];
}

template< typename T, typename MAT, int SIZE >
const qpVec< T, SIZE > & qpMatBase< T, MAT, SIZE >::operator[]( int rowIndex ) const {
	QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < SIZE ) );
	return CRTP().m_vectors[ rowIndex ];
}

template< typename T, typename MAT, int SIZE >
T & qpMatBase< T, MAT, SIZE >::operator()( int row, int column ) {
	QP_ASSERT( ( row > 0 ) && ( row <= SIZE ) );
	QP_ASSERT( ( column > 0 ) && ( column <= SIZE ) );
	return CRTP().m_data[ ( row - 1 ) * SIZE + ( column - 1 ) ];
}

template< typename T, typename MAT, int SIZE >
const T & qpMatBase< T, MAT, SIZE >::operator()( int row, int column ) const {
	QP_ASSERT( ( row > 0 ) && ( row <= SIZE ) );
	QP_ASSERT( ( column > 0 ) && ( column <= SIZE ) );
	return CRTP().m_data[ ( row - 1 ) * SIZE + ( column - 1 ) ];
}

template< typename T, typename MAT, int SIZE >
qpMatBase< T, MAT, SIZE > & qpMatBase< T, MAT, SIZE >::operator+=( const qpMatBase & rhs ) {
	for ( int index = 0; index < NumElements(); ++index ) {
		CRTP().m_data[ index ] += rhs.CRTP().m_data[ index ];
	}
	return *this;
}

template< typename T, typename MAT, int SIZE >
qpMatBase< T, MAT, SIZE > & qpMatBase< T, MAT, SIZE >::operator-=( const qpMatBase & rhs ) {
	for ( int index = 0; index < NumElements(); ++index ) {
		CRTP().m_data[ index ] -= rhs.CRTP().m_data[ index ];
	}
	return *this;
}

template< typename T, typename MAT, int SIZE >
qpMatBase< T, MAT, SIZE > & qpMatBase< T, MAT, SIZE >::operator*=( const qpMatBase & rhs ) {
	for ( int rowIndex = 0; rowIndex < SIZE; ++rowIndex ) {
		qpVec< T, SIZE > row;
		for ( int rhsColumnIndex = 0; rhsColumnIndex < SIZE; ++rhsColumnIndex ) {
			for ( int columnIndex = 0; columnIndex < SIZE; ++columnIndex ) {
				row[ columnIndex ] += CRTP().m_data[( rowIndex * SIZE ) + columnIndex] * rhs.CRTP().m_data[( columnIndex * SIZE ) + rhsColumnIndex];
			}
		}
		CRTP().m_vectors[ rowIndex ] = row;
	}
	return *this;
}

template< typename T, typename MAT, int SIZE >
qpMatBase< T, MAT, SIZE > & qpMatBase< T, MAT, SIZE >::operator*=( const float rhs ) {
	for ( int index = 0; index < NumElements(); ++index ) {
		CRTP().m_data[ index ] *= rhs;
	}
	return *this;
}

template< typename T, typename MAT, int SIZE >
qpMatBase<T, MAT, SIZE> & qpMatBase<T, MAT, SIZE>::Transpose() {
	for ( int rowIndex = 0; rowIndex < NumElements(); ++rowIndex ) {
		for ( int columnIndex = 0; columnIndex < NumElements(); ++columnIndex ) {
			if ( rowIndex == columnIndex ) {
				continue;
			}
			qpSwap( CRTP().m_data[ rowIndex * SIZE + columnIndex ], CRTP().m_data[ columnIndex * SIZE + rowIndex ] );
		}
	}
	return *this;
}

template< typename T, typename MAT, int SIZE >
inline qpMatBase< T, MAT, SIZE > operator+( const qpMatBase< T, MAT, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	qpMatBase< T, MAT, SIZE > result( lhs );
	result += rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline qpMatBase< T, MAT, SIZE > operator-( const qpMatBase< T, MAT, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	qpMatBase< T, MAT, SIZE > result( lhs );
	result -= rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline qpMatBase< T, MAT, SIZE > operator*( const qpMatBase< T, MAT, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	qpMatBase< T, MAT, SIZE > result( lhs );
	result *= rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline qpMatBase< T, MAT, SIZE > operator*( const qpMatBase< T, MAT, SIZE > & lhs, const float rhs ) {
	qpMatBase< T, MAT, SIZE > result( lhs );
	result *= rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline qpMatBase< T, MAT, SIZE > operator*( const float lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	qpMatBase< T, MAT, SIZE > result( rhs );
	result *= lhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline qpVec< T, SIZE > operator*( const qpVec< T, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	qpVec< T, SIZE > result( lhs );
	for ( int rhsColumnIndex = 0; rhsColumnIndex < SIZE; ++rhsColumnIndex ) {
		for ( int columnIndex = 0; columnIndex < SIZE; ++columnIndex ) {
			result[ columnIndex ] += lhs[ columnIndex ] * rhs.CRTP().m_data[ ( columnIndex * SIZE ) + rhsColumnIndex ];
		}
	}
	return result;
}

template < typename T, int SIZE >
class qpMat : public qpMatBase< T, qpMat< T, SIZE >, SIZE > {
public:
	union
	{
		T m_data[ SIZE * SIZE ] {};
		qpVec< T, SIZE > m_vectors [ SIZE ];
		struct {
			T m11;
			T m12;
			T m13;
			T m14;
			T m21;
			T m22;
			T m23;
			T m24;
			T m31;
			T m32;
			T m33;
			T m34;
			T m41;
			T m42;
			T m43;
			T m44;
		};
	};
};

template < typename T >
class qpMat< T, 3 > : public qpMatBase< T, qpMat< T, 3 >, 3 > {
public:
	qpMat()
		: m_data {
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 )
		} {}
	union {
		T m_data[ 3 * 3 ] {};
		qpVec< T, 3 > m_vectors[ 3 ];
		struct {
			T m11;
			T m12;
			T m13;
			T m21;
			T m22;
			T m23;
			T m31;
			T m32;
			T m33;
			T m41;
			T m42;
			T m43;
		};
	};
};

template < typename T >
class qpMat< T, 4 > : public qpMatBase< T, qpMat< T, 4 >, 4 > {
public:
	qpMat()
		: m_data {
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 )
		} {}
	union {
		T m_data[ 4 * 4 ] {};
		qpVec< T, 4 > m_vectors[ 4 ];
		struct {
			T m11;
			T m12;
			T m13;
			T m14;
			T m21;
			T m22;
			T m23;
			T m24;
			T m31;
			T m32;
			T m33;
			T m34;
			T m41;
			T m42;
			T m43;
			T m44;
		};
	};

	qpVec< T, 3 > TransformPoint( const qpVec< T, 3 > & point );
	qpVec< T, 3 > TransformPoint( const qpVec< T, 4 > & point );
	qpVec< T, 3 > TransformVector( const qpVec< T, 3 > & vector );
	qpVec< T, 3 > TransformVector( const qpVec< T, 4 > & vector );
};

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformPoint( const qpVec< T, 3 > & point ) {
	return operator*( qpVec< T, 4 >( point, static_cast< T >( 1 ) ), *this );
}

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformPoint( const qpVec< T, 4 > & point ) {
	return operator*( qpVec< T, 4 >( point.x, point.y, point.z, static_cast< T >( 1 ) ), *this );
}

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformVector( const qpVec< T, 3 > & vector ) {
	return operator*( qpVec< T, 4 >( vector, static_cast< T >( 0 ) ), *this );
}

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformVector( const qpVec< T, 4 > & vector ) {
	return operator*( qpVec< T, 4 >( vector.x, vector.y, vector.z, static_cast< T >( 0 ) ), *this );
}

using qpMat3 = qpMat< float, 3 >;
using qpMat3d = qpMat< double, 3 >;

using qpMat4 = qpMat< float, 4 >;
using qpMat4d = qpMat< double, 4 >;
