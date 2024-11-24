#pragma once
#include "qp_vec.h"

template < typename _type_, typename _matrix_, int _size_ >
class qpMatBase {
public:
	int NumElements() const { return _size_ * _size_; }

	qpVec< _type_, _size_ > & operator[]( int rowIndex );
	const qpVec< _type_, _size_ > & operator[]( int rowIndex ) const;

	_type_ & operator()( int row, int column );
	const _type_ & operator()( int row, int column ) const;

	auto operator<=>( const qpMatBase & other ) const { return memcmp( CRTP().m_data, other.CRTP().m_data, static_cast< uint64 >( _size_ ) * _size_ * sizeof( _type_ ) ); }
	bool operator==( const qpMatBase & other ) const { return ( memcmp( CRTP().m_data, other.CRTP().m_data, static_cast< uint64 >( _size_ ) * _size_ * sizeof( _type_ ) ) == 0 ); }

	qpMatBase & operator+=( const qpMatBase & rhs );
	qpMatBase & operator-=( const qpMatBase & rhs );
	qpMatBase & operator*=( const qpMatBase & rhs );
	qpMatBase & operator*=( const float rhs );

	void Transpose();
	_matrix_ Transposed() const;

	qpVec< _type_, 3 > Forward() const { return static_cast< qpVec< _type_, 3 > >( CRTP().m_rows[ 2 ] ).Normalized(); }
	qpVec< _type_, 3 > Right() const { return static_cast< qpVec< _type_, 3 > >( CRTP().m_rows[ 0 ] ).Normalized(); }
	qpVec< _type_, 3 > Up() const { return static_cast< qpVec< _type_, 3 > >( CRTP().m_rows[ 1 ] ).Normalized(); }

	const qpVec< _type_, _size_ > & Row( int rowIndex ) const { return CRTP().m_rows[ rowIndex ]; }
	qpVec< _type_, _size_ > Column( int columnIndex ) const;

	operator _matrix_ &() { return CRTP(); }
	operator const _matrix_ &() const { return CRTP(); }

	_type_ * Data() const { return CRTP().m_data; }

	_matrix_ & CRTP() { return static_cast< _matrix_ & >( *this ); }
	const _matrix_ & CRTP() const { return static_cast< const _matrix_ & >( *this ); }
};

template< typename _type_, typename _matrix_, int _size_ >
qpVec< _type_, _size_ > & qpMatBase< _type_, _matrix_, _size_ >::operator[]( int rowIndex ) {
	QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < _size_ ) );
	return CRTP().m_rows[ rowIndex ];
}

template< typename _type_, typename _matrix_, int _size_ >
const qpVec< _type_, _size_ > & qpMatBase< _type_, _matrix_, _size_ >::operator[]( int rowIndex ) const {
	QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < _size_ ) );
	return CRTP().m_rows[ rowIndex ];
}

template< typename _type_, typename _matrix_, int _size_ >
_type_ & qpMatBase< _type_, _matrix_, _size_ >::operator()( int row, int column ) {
	QP_ASSERT( ( row > 0 ) && ( row <= _size_ ) );
	QP_ASSERT( ( column > 0 ) && ( column <= _size_ ) );
	return CRTP().m_data[ ( row - 1 ) * _size_ + ( column - 1 ) ];
}

template< typename _type_, typename _matrix_, int _size_ >
const _type_ & qpMatBase< _type_, _matrix_, _size_ >::operator()( int row, int column ) const {
	QP_ASSERT( ( row > 0 ) && ( row <= _size_ ) );
	QP_ASSERT( ( column > 0 ) && ( column <= _size_ ) );
	return CRTP().m_data[ ( row - 1 ) * _size_ + ( column - 1 ) ];
}

template< typename _type_, typename _matrix_, int _size_ >
qpMatBase< _type_, _matrix_, _size_ > & qpMatBase< _type_, _matrix_, _size_ >::operator+=( const qpMatBase & rhs ) {
	for ( int index = 0; index < NumElements(); ++index ) {
		CRTP().m_data[ index ] += rhs.CRTP().m_data[ index ];
	}
	return *this;
}

template< typename _type_, typename _matrix_, int _size_ >
qpMatBase< _type_, _matrix_, _size_ > & qpMatBase< _type_, _matrix_, _size_ >::operator-=( const qpMatBase & rhs ) {
	for ( int index = 0; index < NumElements(); ++index ) {
		CRTP().m_data[ index ] -= rhs.CRTP().m_data[ index ];
	}
	return *this;
}

template< typename _type_, typename _matrix_, int _size_ >
qpMatBase< _type_, _matrix_, _size_ > & qpMatBase< _type_, _matrix_, _size_ >::operator*=( const qpMatBase & rhs ) {
	for ( int columnIndex = 0; columnIndex < _size_; ++columnIndex ) {
		for ( int rowIndex = 0; rowIndex < _size_; ++rowIndex ) {
			CRTP().m_rows[ rowIndex ][ columnIndex ] = qpDot( Row( rowIndex ), rhs.CRTP().Column( columnIndex ) );
		}
	}
	return *this;
}

template< typename _type_, typename _matrix_, int _size_ >
qpMatBase< _type_, _matrix_, _size_ > & qpMatBase< _type_, _matrix_, _size_ >::operator*=( const float rhs ) {
	for ( int index = 0; index < NumElements(); ++index ) {
		CRTP().m_data[ index ] *= rhs;
	}
	return *this;
}

template< typename _type_, typename _matrix_, int _size_ >
void qpMatBase< _type_, _matrix_, _size_ >::Transpose() {
	for ( int rowIndex = 0; rowIndex < _size_; ++rowIndex ) {
		for ( int columnIndex = 0; columnIndex < _size_; ++columnIndex ) {
			if ( rowIndex == columnIndex ) {
				continue;
			}
			qpSwap( CRTP().m_data[ rowIndex * _size_ + columnIndex ], CRTP().m_data[ columnIndex * _size_ + rowIndex ] );
		}
	}
}

template< typename _type_, typename _matrix_, int _size_ >
_matrix_ qpMatBase< _type_, _matrix_, _size_ >::Transposed() const {
	return qpTranspose( *this );
}

template< typename _type_, typename _matrix_, int _size_ >
qpVec< _type_, _size_ > qpMatBase< _type_, _matrix_, _size_ >::Column( int columnIndex ) const {
	qpVec< _type_, _size_ > column;
	for ( int rowIndex = 0; rowIndex < _size_; ++rowIndex ) {
		column[ rowIndex ] = CRTP().m_rows[ rowIndex ][ columnIndex ];
	}
	return column;
}

template< typename _type_, typename _matrix_, int _size_ >
inline _matrix_ operator+( const qpMatBase< _type_, _matrix_, _size_ > & lhs, const qpMatBase< _type_, _matrix_, _size_ > & rhs ) {
	_matrix_ result( lhs );
	result += rhs;
	return result;
}

template< typename _type_, typename _matrix_, int _size_ >
inline _matrix_ operator-( const qpMatBase< _type_, _matrix_, _size_ > & lhs, const qpMatBase< _type_, _matrix_, _size_ > & rhs ) {
	_matrix_ result( lhs );
	result -= rhs;
	return result;
}

template< typename _type_, typename _matrix_, int _size_ >
inline _matrix_ operator*( const qpMatBase< _type_, _matrix_, _size_ > & lhs, const qpMatBase< _type_, _matrix_, _size_ > & rhs ) {
	_matrix_ result( lhs );
	result *= rhs;
	return result;
}

template< typename _type_, typename _matrix_, int _size_ >
inline _matrix_ operator*( const qpMatBase< _type_, _matrix_, _size_ > & lhs, const float rhs ) {
	_matrix_ result( lhs );
	result *= rhs;
	return result;
}

template< typename _type_, typename _matrix_, int _size_ >
inline _matrix_ operator*( const float lhs, const qpMatBase< _type_, _matrix_, _size_ > & rhs ) {
	_matrix_ result( rhs );
	result *= lhs;
	return result;
}

template< typename _type_, typename _matrix_, int _size_ >
inline qpVec< _type_, _size_ > operator*( const qpVec< _type_, _size_ > & lhs, const qpMatBase< _type_, _matrix_, _size_ > & rhs ) {
	qpVec< _type_, _size_ > result( lhs );
	for ( int columnIndex = 0; columnIndex < _size_; ++columnIndex ) {
		result[ columnIndex ] = qpDot( lhs, rhs.CRTP().Column( columnIndex ) );
	}
	return result;
}

template < typename _type_, int _size_ >
class qpMat : public qpMatBase< _type_, qpMat< _type_, _size_ >, _size_ > {
public:
	union {
		_type_ m_data[ _size_ * _size_ ] {};
		qpVec< _type_, _size_ > m_rows [ _size_ ];
	};
};

template < typename _type_ >
class qpMat< _type_, 3 > : public qpMatBase< _type_, qpMat< _type_, 3 >, 3 > {
public:
	qpMat()
		: m_data {
			static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ),
			static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ),
			static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 )
		} {}
	qpMat(	const float m11, const float m12, const float m13,
			const float m21, const float m22, const float m23,
			const float m31, const float m32, const float m33 )
				: m_data {
					 m11, m12, m13,
					 m21, m22, m23,
					 m31, m32, m33
				} { }
	qpMat( const qpVec< _type_, 3 > rowA, const qpVec< _type_, 3 > rowB, const qpVec< _type_, 3 > rowC )
		: m_rows { rowA, rowB, rowC } { }
	union {
		_type_ m_data[ 3 * 3 ] {};
		qpVec< _type_, 3 > m_rows[ 3 ];
	};
};

template < typename _type_ >
class qpMat< _type_, 4 > : public qpMatBase< _type_, qpMat< _type_, 4 >, 4 > {
public:
	qpMat()
		: m_data {
			static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ),
			static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ),
			static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ),
			static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 )
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
	qpMat( const qpVec< _type_, 4 > rowA, const qpVec< _type_, 4 > rowB, const qpVec< _type_, 4 > rowC, const qpVec< _type_, 4 > rowD )
		: m_rows { rowA, rowB, rowC, rowD } {}
	union {
		_type_ m_data[ 4 * 4 ] {};
		qpVec< _type_, 4 > m_rows[ 4 ];
	};

	qpVec< _type_, 3 > TransformPoint( const qpVec< _type_, 3 > & point );
	qpVec< _type_, 3 > TransformPoint( const qpVec< _type_, 4 > & point );
	qpVec< _type_, 3 > TransformVector( const qpVec< _type_, 3 > & vector );
	qpVec< _type_, 3 > TransformVector( const qpVec< _type_, 4 > & vector );
};

template< typename _type_ >
qpVec< _type_, 3 > qpMat< _type_, 4 >::TransformPoint( const qpVec< _type_, 3 > & point ) {
	return static_cast< qpVec< _type_, 3 > >( operator*( qpVec< _type_, 4 >( point, static_cast< _type_ >( 1 ) ), *this ) );
}

template< typename _type_ >
qpVec< _type_, 3 > qpMat< _type_, 4 >::TransformPoint( const qpVec< _type_, 4 > & point ) {
	return static_cast< qpVec< _type_, 3 > >( operator*( qpVec< _type_, 4 >( point.x, point.y, point.z, static_cast< _type_ >( 1 ) ), *this ) );
}

template< typename _type_ >
qpVec< _type_, 3 > qpMat< _type_, 4 >::TransformVector( const qpVec< _type_, 3 > & vector ) {
	return static_cast< qpVec< _type_, 3 > >( operator*( qpVec< _type_, 4 >( vector, static_cast< _type_ >( 0 ) ), *this ) );
}

template< typename _type_ >
qpVec< _type_, 3 > qpMat< _type_, 4 >::TransformVector( const qpVec< _type_, 4 > & vector ) {
	return static_cast< qpVec< _type_, 3 > >( operator*( qpVec< _type_, 4 >( vector.x, vector.y, vector.z, static_cast< _type_ >( 0 ) ), *this ) );
}

template< typename _type_ >
qpMat< _type_, 4 > qpPerspectiveProjectionMatrix( const _type_ & hFovDeg, const int width, const int height, const _type_ & nearClip, const _type_ & farClip ) {
	const _type_ hFovRad = qpMath::Deg2Rad( hFovDeg );
	const _type_ vFovRad = static_cast< _type_ >( 2.0 ) * qpMath::Atan< _type_ >( qpMath::Tan< _type_ >( hFovRad / static_cast< _type_ >( 2.0 ) ) );

	const _type_ aspectRatio = static_cast< _type_ >( height ) / static_cast< _type_ >( width );

	const _type_ A = farClip / ( farClip - nearClip );
	qpMat< _type_, 4 > projectionMatrix;
	projectionMatrix( 1, 1 ) = static_cast< _type_ >( 1.0 ) / qpMath::Tan< _type_ >( hFovRad * static_cast< _type_ >( 0.5 ) );
	projectionMatrix( 2, 2 ) = static_cast< _type_ >( 1.0 ) / qpMath::Tan< _type_ >( vFovRad * static_cast< _type_ >( 0.5 ) * aspectRatio );
	projectionMatrix( 3, 3 ) = A;
	projectionMatrix( 3, 4 ) = static_cast< _type_ >( 1.0 ) / A;
	projectionMatrix( 4, 3 ) = A * -nearClip;
	projectionMatrix( 4, 4 ) = static_cast< _type_ >( 0.0 );
	return projectionMatrix;
}
template< typename _type_ >
qpMat< _type_, 4 > qpOrthographicProjectionMatrix( const _type_ & left, const _type_ & right, const _type_ & bottom, const _type_ & top, const _type_ & near, const _type_ & far ) {
	qpMat< _type_, 4 > projection;
	projection( 1, 1 ) = static_cast< _type_ >( 2.0 ) / ( right - left );
	projection( 2, 2 ) = static_cast< _type_ >( 2.0 ) / ( top - bottom );
	projection( 3, 3 ) = static_cast< _type_ >( 1.0 ) / ( far - near );
	projection( 4, 1 ) = static_cast< _type_ >( -( ( right + left ) / ( right - left ) ) );
	projection( 4, 2 ) = static_cast< _type_ >( -( ( top + bottom ) / ( top - bottom ) ) );
	projection( 4, 3 ) = static_cast< _type_ >( -( near / ( near - far ) ) );
	projection( 4, 4 ) = static_cast< _type_ >( 1 );
	return projection;
}
template< typename _type_ >
qpMat< _type_, 4 > qpAffineInverse( const qpMat< _type_, 4 > & matrix ) {
	_type_ detA = matrix( 1, 1 ) * ( ( matrix( 2, 2 ) * matrix( 3, 3 ) ) - ( matrix( 2, 3 ) * matrix( 3, 2 ) ) );
	_type_ detB = matrix( 1, 2 ) * ( ( matrix( 2, 1 ) * matrix( 3, 3 ) ) - ( matrix( 2, 3 ) * matrix( 3, 1 ) ) );
	_type_ detC = matrix( 1, 3 ) * ( ( matrix( 2, 1 ) * matrix( 3, 2 ) ) - ( matrix( 2, 2 ) * matrix( 3, 1 ) ) );
	_type_ det = detA - detB + detC;

	QP_ASSERT_MSG( det != static_cast< _type_ >( 0 ), "Matrix does not have an inverse!" );

	_type_ det11 = ( matrix( 2, 2 ) * matrix( 3, 3 ) - matrix( 2, 3 ) * matrix( 3, 2 ) );
	_type_ det12 = -( matrix( 2, 1 ) * matrix( 3, 3 ) - matrix( 2, 3 ) * matrix( 3, 1 ) );
	_type_ det13 = ( matrix( 2, 1 ) * matrix( 3, 2 ) - matrix( 2, 2 ) * matrix( 3, 1 ) );

	_type_ det21 = -( matrix( 1, 2 ) * matrix( 3, 3 ) - matrix( 1, 3 ) * matrix( 3, 2 ) );
	_type_ det22 = ( matrix( 1, 1 ) * matrix( 3, 3 ) - matrix( 1, 3 ) * matrix( 3, 1 ) );
	_type_ det23 = -( matrix( 1, 1 ) * matrix( 3, 2 ) - matrix( 1, 2 ) * matrix( 3, 1 ) );

	_type_ det31 = ( matrix( 1, 2 ) * matrix( 2, 3 ) - matrix( 1, 3 ) * matrix( 2, 2 ) );
	_type_ det32 = -( matrix( 1, 1 ) * matrix( 2, 3 ) - matrix( 1, 3 ) * matrix( 2, 1 ) );
	_type_ det33 = ( matrix( 1, 1 ) * matrix( 2, 2 ) - matrix( 1, 2 ) * matrix( 2, 1 ) );

	qpMat< _type_, 4 > inverse;

	const _type_ oneOverDet = static_cast< _type_ >( 1.0 ) / det;
	inverse( 1, 1 ) = det11 * oneOverDet;
	inverse( 1, 2 ) = det21 * oneOverDet;
	inverse( 1, 3 ) = det31 * oneOverDet;

	inverse( 2, 1 ) = det12 * oneOverDet;
	inverse( 2, 2 ) = det22 * oneOverDet;
	inverse( 2, 3 ) = det32 * oneOverDet;

	inverse( 3, 1 ) = det13 * oneOverDet;
	inverse( 3, 2 ) = det23 * oneOverDet;
	inverse( 3, 3 ) = det33 * oneOverDet;

	const qpVec< _type_, 4 > & translation = -matrix[ 3 ];
	inverse[ 3 ] = qpVec< _type_, 4 > ( inverse.TransformPoint( translation ), static_cast< _type_ >( 1.0 ) );

	return inverse;
}

template< typename _type_ >
qpMat< _type_, 4 > qpRotationAndTranslationInverse( const qpMat< _type_, 4 > & matrix ) {
	qpMat< _type_, 4 > inverse;
	inverse( 1, 1 ) = matrix( 1, 1 );
	inverse( 1, 2 ) = matrix( 2, 1 );
	inverse( 1, 3 ) = matrix( 3, 1 );

	inverse( 2, 1 ) = matrix( 1, 2 );
	inverse( 2, 2 ) = matrix( 2, 2 );
	inverse( 2, 3 ) = matrix( 3, 2 );

	inverse( 3, 1 ) = matrix( 1, 3 );
	inverse( 3, 2 ) = matrix( 2, 3 );
	inverse( 3, 3 ) = matrix( 3, 3 );

	inverse[ 3 ] = qpVec< _type_, 4 >( inverse.TransformPoint( static_cast< qpVec< _type_, 3 > >( -matrix[ 3 ] ) ), static_cast< _type_ >( 1.0 ) );

	return inverse;
}

template< typename _type_ >
qpMat< _type_, 4 > qpCreateTranslation( const qpVec< _type_, 3 > & translation ) {
	qpMat< _type_, 4 > translationMatrix;
	translationMatrix( 4, 1 ) = translation.x;
	translationMatrix( 4, 2 ) = translation.y;
	translationMatrix( 4, 3 ) = translation.z;
	return translationMatrix;
}

template< typename _type_ >
qpMat< _type_, 4 > qpCreateScale( const qpVec< _type_, 3 > & scale ) {
	qpMat< _type_, 4 > scaleMatrix;
	scaleMatrix( 1, 1 ) = scale.x;
	scaleMatrix( 2, 2 ) = scale.y;
	scaleMatrix( 3, 3 ) = scale.z;
	return scaleMatrix;
}


template< typename _type_ >
qpMat< _type_, 4 > qpCreateRotationX( const _type_ angleDeg ) {
	const _type_ angleRad = qpMath::Deg2Rad( angleDeg );
	qpMat< _type_, 4 > rotationMatrix;
	rotationMatrix( 2, 2 ) = qpMath::Cos( angleRad );
	rotationMatrix( 2, 3 ) = qpMath::Sin( angleRad );
	rotationMatrix( 3, 3 ) = qpMath::Cos( angleRad );
	rotationMatrix( 3, 2 ) = -qpMath::Sin( angleRad );
	return rotationMatrix;
}

template< typename _type_ >
qpMat< _type_, 4 > qpCreateRotationY( const _type_ angleDeg ) {
	const _type_ angleRad = qpMath::Deg2Rad( angleDeg );
	qpMat< _type_, 4 > rotationMatrix;
	rotationMatrix( 1, 1 ) = qpMath::Cos( angleRad );
	rotationMatrix( 1, 3 ) = -qpMath::Sin( angleRad );
	rotationMatrix( 3, 1 ) = qpMath::Sin( angleRad );
	rotationMatrix( 3, 3 ) = qpMath::Cos( angleRad );
	return rotationMatrix;
}

template< typename _type_ >
qpMat< _type_, 4 > qpCreateRotationZ( const _type_ angleDeg ) {
	const _type_ angleRad = qpMath::Deg2Rad( angleDeg );
	qpMat< _type_, 4 > rotationMatrix;
	rotationMatrix( 1, 1 ) = qpMath::Cos( angleRad );
	rotationMatrix( 1, 2 ) = qpMath::Sin( angleRad );
	rotationMatrix( 2, 1 ) = -qpMath::Sin( angleRad );
	rotationMatrix( 2, 2 ) = qpMath::Cos( angleRad );
	return rotationMatrix;
}

template< typename _type_, typename _matrix_, int _size_ >
_matrix_ qpTranspose( const qpMatBase< _type_, _matrix_, _size_ > & matrix ) {
	_matrix_ transposed = matrix;
	transposed.Transpose();
	return transposed;
}

using qpMat3 = qpMat< float, 3 >;
using qpMat3d = qpMat< double, 3 >;

using qpMat4 = qpMat< float, 4 >;
using qpMat4d = qpMat< double, 4 >;

static const qpMat3 g_mat3Identity {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};
static const qpMat4 g_mat4Identity {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
static const qpMat4 g_mat4NaN {
	NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN
};
