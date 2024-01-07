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

	auto operator<=>( const qpMatBase & other ) const { return memcmp( CRTP().m_data, other.CRTP().m_data, static_cast< uint64 >( SIZE ) * SIZE * sizeof( T ) ); }
	bool operator==( const qpMatBase & other ) const { return ( memcmp( CRTP().m_data, other.CRTP().m_data, static_cast< uint64 >( SIZE ) * SIZE * sizeof( T ) ) == 0 ); }

	qpMatBase & operator+=( const qpMatBase & rhs );
	qpMatBase & operator-=( const qpMatBase & rhs );
	qpMatBase & operator*=( const qpMatBase & rhs );
	qpMatBase & operator*=( const float rhs );

	void Transpose();
	MAT Transposed();

	qpVec< T, 3 > Forward() const { return static_cast< qpVec< T, 3 > >( CRTP().m_rows[ 2 ] ).Normalized(); }
	qpVec< T, 3 > Right() const { return static_cast< qpVec< T, 3 > >( CRTP().m_rows[ 0 ] ).Normalized(); }
	qpVec< T, 3 > Up() const { return static_cast< qpVec< T, 3 > >( CRTP().m_rows[ 1 ] ).Normalized(); }

	const qpVec< T, SIZE > & Row( int rowIndex ) const { return CRTP().m_rows[ rowIndex ]; }
	qpVec< T, SIZE > Column( int columnIndex ) const;

	operator MAT &() { return CRTP(); }
	operator const MAT &() const { return CRTP(); }

	T * Data() const { return CRTP().m_data; }

	MAT & CRTP() { return static_cast< MAT & >( *this ); }
	const MAT & CRTP() const { return static_cast< const MAT & >( *this ); }
};

template< typename T, typename MAT, int SIZE >
qpVec< T, SIZE > & qpMatBase< T, MAT, SIZE >::operator[]( int rowIndex ) {
	QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < SIZE ) );
	return CRTP().m_rows[ rowIndex ];
}

template< typename T, typename MAT, int SIZE >
const qpVec< T, SIZE > & qpMatBase< T, MAT, SIZE >::operator[]( int rowIndex ) const {
	QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < SIZE ) );
	return CRTP().m_rows[ rowIndex ];
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
	for ( int columnIndex = 0; columnIndex < SIZE; ++columnIndex ) {
		for ( int rowIndex = 0; rowIndex < SIZE; ++rowIndex ) {
			CRTP().m_rows[ rowIndex ][ columnIndex ] = qpDot( Row( rowIndex ), rhs.CRTP().Column( columnIndex ) );
		}
	}

	/*for ( int rowIndex = 0; rowIndex < SIZE; ++rowIndex ) {
		qpVec< T, SIZE > row;
		for ( int rhsColumnIndex = 0; rhsColumnIndex < SIZE; ++rhsColumnIndex ) {
			for ( int columnIndex = 0; columnIndex < SIZE; ++columnIndex ) {
				row[ columnIndex ] += CRTP().m_data[ ( rowIndex * SIZE ) + columnIndex ] * rhs.CRTP().m_data[ ( columnIndex * SIZE ) + rhsColumnIndex ];
			}
		}
		CRTP().m_rows[ rowIndex ] = row;
	}*/
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
void qpMatBase< T, MAT, SIZE >::Transpose() {
	for ( int rowIndex = 0; rowIndex < SIZE; ++rowIndex ) {
		for ( int columnIndex = 0; columnIndex < SIZE; ++columnIndex ) {
			if ( rowIndex == columnIndex ) {
				continue;
			}
			qpSwap( CRTP().m_data[ rowIndex * SIZE + columnIndex ], CRTP().m_data[ columnIndex * SIZE + rowIndex ] );
		}
	}
}

template< typename T, typename MAT, int SIZE >
MAT qpMatBase< T, MAT, SIZE >::Transposed() {
	return qpTranspose( *this );
}

template< typename T, typename MAT, int SIZE >
qpVec< T, SIZE > qpMatBase< T, MAT, SIZE >::Column( int columnIndex ) const {
	qpVec< T, SIZE > column;
	for ( int rowIndex = 0; rowIndex < SIZE; ++rowIndex ) {
		column[ rowIndex ] = CRTP().m_rows[ rowIndex ][ columnIndex ];
	}
	return column;
}

template< typename T, typename MAT, int SIZE >
inline MAT operator+( const qpMatBase< T, MAT, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	MAT result( lhs );
	result += rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline MAT operator-( const qpMatBase< T, MAT, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	MAT result( lhs );
	result -= rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline MAT operator*( const qpMatBase< T, MAT, SIZE > & lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	MAT result( lhs );
	result *= rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline MAT operator*( const qpMatBase< T, MAT, SIZE > & lhs, const float rhs ) {
	MAT result( lhs );
	result *= rhs;
	return result;
}

template< typename T, typename MAT, int SIZE >
inline MAT operator*( const float lhs, const qpMatBase< T, MAT, SIZE > & rhs ) {
	MAT result( rhs );
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
	union {
		T m_data[ SIZE * SIZE ] {};
		qpVec< T, SIZE > m_rows [ SIZE ];
	};
};

template < typename T >
class qpMat< T, 3 > : public qpMatBase< T, qpMat< T, 3 >, 3 > {
public:
	qpMat()
		: m_data {
			static_cast< T >( 1 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 1 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 1 )
		} {}
	qpMat(	const float m11, const float m12, const float m13,
			const float m21, const float m22, const float m23,
			const float m31, const float m32, const float m33 )
				: m_data {
					 m11, m12, m13,
					 m21, m22, m23,
					 m31, m32, m33
				} { }
	qpMat( const qpVec< T, 3 > rowA, const qpVec< T, 3 > rowB, const qpVec< T, 3 > rowC )
		: m_rows { rowA, rowB, rowC } { }
	union {
		T m_data[ 3 * 3 ] {};
		qpVec< T, 3 > m_rows[ 3 ];
	};
};

template < typename T >
class qpMat< T, 4 > : public qpMatBase< T, qpMat< T, 4 >, 4 > {
public:
	qpMat()
		: m_data {
			static_cast< T >( 1 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 1 ), static_cast< T >( 0 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 1 ), static_cast< T >( 0 ),
			static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 1 )
		} {}
	qpMat( const float m11, const float m12, const float m13, const float m14,
		   const float m21, const float m22, const float m23, const float m24,
		   const float m31, const float m32, const float m33, const float m34,
		   const float m41, const float m42, const float m43, const float m44 )
			   : m_data {
					m11, m12, m13, m14,
					m21, m22, m23, m24,
					m31, m32, m33, m34,
					m41, m42, m43, m44
			   } {}
	qpMat( const qpVec< T, 4 > rowA, const qpVec< T, 4 > rowB, const qpVec< T, 4 > rowC, const qpVec< T, 4 > rowD )
		: m_rows { rowA, rowB, rowC, rowD } {}
	union {
		T m_data[ 4 * 4 ] {};
		qpVec< T, 4 > m_rows[ 4 ];
	};

	qpVec< T, 3 > TransformPoint( const qpVec< T, 3 > & point );
	qpVec< T, 3 > TransformPoint( const qpVec< T, 4 > & point );
	qpVec< T, 3 > TransformVector( const qpVec< T, 3 > & vector );
	qpVec< T, 3 > TransformVector( const qpVec< T, 4 > & vector );
};

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformPoint( const qpVec< T, 3 > & point ) {
	return static_cast< qpVec< T, 3 > >( operator*( qpVec< T, 4 >( point, static_cast< T >( 1 ) ), *this ) );
}

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformPoint( const qpVec< T, 4 > & point ) {
	return static_cast< qpVec< T, 3 > >( operator*( qpVec< T, 4 >( point.x, point.y, point.z, static_cast< T >( 1 ) ), *this ) );
}

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformVector( const qpVec< T, 3 > & vector ) {
	return static_cast< qpVec< T, 3 > >( operator*( qpVec< T, 4 >( vector, static_cast< T >( 0 ) ), *this ) );
}

template< typename T >
qpVec< T, 3 > qpMat< T, 4 >::TransformVector( const qpVec< T, 4 > & vector ) {
	return static_cast< qpVec< T, 3 > >( operator*( qpVec< T, 4 >( vector.x, vector.y, vector.z, static_cast< T >( 0 ) ), *this ) );
}

template< typename T >
qpMat< T, 4 > qpPerspectiveProjectionMatrix( const T & hFovDeg, const int width, const int height, const T & nearClip, const T & farClip ) {
	const T hFovRad = qpMath::Deg2Rad( hFovDeg );
	const T vFovRad = static_cast< T >( 2.0 ) * qpMath::Atan< T >( qpMath::Tan< T >( hFovRad / static_cast< T >( 2.0 ) ) );

	const T aspectRatio = static_cast< T >( height ) / static_cast< T >( width );

	const T A = farClip / ( farClip - nearClip );
	qpMat< T, 4 > projectionMatrix;
	projectionMatrix( 1, 1 ) = static_cast< T >( 1.0 ) / qpMath::Tan< T >( hFovRad * static_cast< T >( 0.5 ) );
	projectionMatrix( 2, 2 ) = static_cast< T >( 1.0 ) / qpMath::Tan< T >( vFovRad * static_cast< T >( 0.5 ) * aspectRatio );
	projectionMatrix( 3, 3 ) = A;
	projectionMatrix( 3, 4 ) = static_cast< T >( 1.0 ) / A;
	projectionMatrix( 4, 3 ) = A * -nearClip;
	projectionMatrix( 4, 4 ) = static_cast< T >( 0.0 );
	return projectionMatrix;
}
template< typename T >
qpMat< T, 4 > qpOrthographicProjectionMatrix( const T & left, const T & right, const T & bottom, const T & top, const T & near, const T & far ) {
	qpMat< T, 4 > projection;
	projection( 1, 1 ) = static_cast< T >( 2.0 ) / ( right - left );
	projection( 2, 2 ) = static_cast< T >( 2.0 ) / ( top - bottom );
	projection( 3, 3 ) = static_cast< T >( 1.0 ) / ( far - near );
	projection( 4, 1 ) = static_cast< T >( -( ( right + left ) / ( right - left ) ) );
	projection( 4, 2 ) = static_cast< T >( -( ( top + bottom ) / ( top - bottom ) ) );
	projection( 4, 3 ) = static_cast< T >( -( near / ( near - far ) ) );
	projection( 4, 4 ) = static_cast< T >( 1 );
	return projection;
}
template< typename T >
qpMat< T, 4 > qpAffineInverse( const qpMat< T, 4 > & matrix ) {
	T detA = matrix( 1, 1 ) * ( ( matrix( 2, 2 ) * matrix( 3, 3 ) ) - ( matrix( 2, 3 ) * matrix( 3, 2 ) ) );
	T detB = matrix( 1, 2 ) * ( ( matrix( 2, 1 ) * matrix( 3, 3 ) ) - ( matrix( 2, 3 ) * matrix( 3, 1 ) ) );
	T detC = matrix( 1, 3 ) * ( ( matrix( 2, 1 ) * matrix( 3, 2 ) ) - ( matrix( 2, 2 ) * matrix( 3, 1 ) ) );
	T det = detA - detB + detC;

	QP_ASSERT_MSG( det != static_cast< T >( 0 ), "Matrix does not have an inverse!" );

	T det11 = ( matrix( 2, 2 ) * matrix( 3, 3 ) - matrix( 2, 3 ) * matrix( 3, 2 ) );
	T det12 = -( matrix( 2, 1 ) * matrix( 3, 3 ) - matrix( 2, 3 ) * matrix( 3, 1 ) );
	T det13 = ( matrix( 2, 1 ) * matrix( 3, 2 ) - matrix( 2, 2 ) * matrix( 3, 1 ) );

	T det21 = -( matrix( 1, 2 ) * matrix( 3, 3 ) - matrix( 1, 3 ) * matrix( 3, 2 ) );
	T det22 = ( matrix( 1, 1 ) * matrix( 3, 3 ) - matrix( 1, 3 ) * matrix( 3, 1 ) );
	T det23 = -( matrix( 1, 1 ) * matrix( 3, 2 ) - matrix( 1, 2 ) * matrix( 3, 1 ) );

	T det31 = ( matrix( 1, 2 ) * matrix( 2, 3 ) - matrix( 1, 3 ) * matrix( 2, 2 ) );
	T det32 = -( matrix( 1, 1 ) * matrix( 2, 3 ) - matrix( 1, 3 ) * matrix( 2, 1 ) );
	T det33 = ( matrix( 1, 1 ) * matrix( 2, 2 ) - matrix( 1, 2 ) * matrix( 2, 1 ) );

	qpMat< T, 4 > inverse;

	const T oneOverDet = static_cast< T >( 1.0 ) / det;
	inverse( 1, 1 ) = det11 * oneOverDet;
	inverse( 1, 2 ) = det21 * oneOverDet;
	inverse( 1, 3 ) = det31 * oneOverDet;

	inverse( 2, 1 ) = det12 * oneOverDet;
	inverse( 2, 2 ) = det22 * oneOverDet;
	inverse( 2, 3 ) = det32 * oneOverDet;

	inverse( 3, 1 ) = det13 * oneOverDet;
	inverse( 3, 2 ) = det23 * oneOverDet;
	inverse( 3, 3 ) = det33 * oneOverDet;

	const qpVec< T, 4 > & translation = -matrix[ 3 ];
	inverse[ 3 ] = qpVec< T, 4 > ( inverse.TransformPoint( translation ), static_cast< T >( 1.0 ) );

	return inverse;
}

template< typename T >
qpMat< T, 4 > qpRotationAndTranslationInverse( const qpMat< T, 4 > & matrix ) {
	qpMat< T, 4 > inverse;
	inverse( 1, 1 ) = matrix( 1, 1 );
	inverse( 1, 2 ) = matrix( 2, 1 );
	inverse( 1, 3 ) = matrix( 3, 1 );

	inverse( 2, 1 ) = matrix( 1, 2 );
	inverse( 2, 2 ) = matrix( 2, 2 );
	inverse( 2, 3 ) = matrix( 3, 2 );

	inverse( 3, 1 ) = matrix( 1, 3 );
	inverse( 3, 2 ) = matrix( 2, 3 );
	inverse( 3, 3 ) = matrix( 3, 3 );

	inverse[ 3 ] = qpVec< T, 4 >( inverse.TransformPoint( static_cast< qpVec< T, 3 > >( -matrix[ 3 ] ) ), static_cast< T >( 1.0 ) );

	return inverse;
}

template< typename T >
qpMat< T, 4 > qpCreateTranslation( const qpVec< T, 3 > & translation ) {
	qpMat< T, 4 > translationMatrix;
	translationMatrix( 4, 1 ) = translation.x;
	translationMatrix( 4, 2 ) = translation.y;
	translationMatrix( 4, 3 ) = translation.z;
	return translationMatrix;
}

template< typename T >
qpMat< T, 4 > qpCreateScale( const qpVec< T, 3 > & scale ) {
	qpMat< T, 4 > scaleMatrix;
	scaleMatrix( 1, 1 ) = scale.x;
	scaleMatrix( 2, 2 ) = scale.y;
	scaleMatrix( 3, 3 ) = scale.z;
	return scaleMatrix;
}


template< typename T >
qpMat< T, 4 > qpCreateRotationX( const T angleDeg ) {
	const T angleRad = qpMath::Deg2Rad( angleDeg );
	qpMat< T, 4 > rotationMatrix;
	rotationMatrix( 2, 2 ) = qpMath::Cos( angleRad );
	rotationMatrix( 2, 3 ) = qpMath::Sin( angleRad );
	rotationMatrix( 3, 3 ) = qpMath::Cos( angleRad );
	rotationMatrix( 3, 2 ) = -qpMath::Sin( angleRad );
	return rotationMatrix;
}

template< typename T >
qpMat< T, 4 > qpCreateRotationY( const T angleDeg ) {
	const T angleRad = qpMath::Deg2Rad( angleDeg );
	qpMat< T, 4 > rotationMatrix;
	rotationMatrix( 1, 1 ) = qpMath::Cos( angleRad );
	rotationMatrix( 1, 3 ) = -qpMath::Sin( angleRad );
	rotationMatrix( 3, 1 ) = qpMath::Sin( angleRad );
	rotationMatrix( 3, 3 ) = qpMath::Cos( angleRad );
	return rotationMatrix;
}

template< typename T >
qpMat< T, 4 > qpCreateRotationZ( const T angleDeg ) {
	const T angleRad = qpMath::Deg2Rad( angleDeg );
	qpMat< T, 4 > rotationMatrix;
	rotationMatrix( 1, 1 ) = qpMath::Cos( angleRad );
	rotationMatrix( 1, 2 ) = qpMath::Sin( angleRad );
	rotationMatrix( 2, 1 ) = -qpMath::Sin( angleRad );
	rotationMatrix( 2, 2 ) = qpMath::Cos( angleRad );
	return rotationMatrix;
}

template< typename T, typename MAT, int SIZE >
MAT qpTranspose( const qpMatBase< T, MAT, SIZE > & matrix ) {
	MAT transposed = matrix;
	transposed.Transpose();
	return transposed;
}

using qpMat3 = qpMat< float, 3 >;
using qpMat3d = qpMat< double, 3 >;

using qpMat4 = qpMat< float, 4 >;
using qpMat4d = qpMat< double, 4 >;

static qpMat3 g_Mat3Identity {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};
static qpMat4 g_Mat4Identity {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
