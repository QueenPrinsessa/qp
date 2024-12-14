#pragma once
#include "qp_vec.h"

namespace qp {
	template < typename _type_, typename _matrix_, int _size_ >
	class MatBase {
	public:
		int NumElements() const { return _size_ * _size_; }

		Vec< _type_, _size_ > & operator[]( int rowIndex );
		const Vec< _type_, _size_ > & operator[]( int rowIndex ) const;

		_type_ & operator()( int row, int column );
		const _type_ & operator()( int row, int column ) const;

		auto operator<=>( const MatBase & other ) const { return memcmp( CRTP().m_data, other.CRTP().m_data, static_cast< uint64 >( _size_ ) * _size_ * sizeof( _type_ ) ); }
		bool operator==( const MatBase & other ) const { return ( memcmp( CRTP().m_data, other.CRTP().m_data, static_cast< uint64 >( _size_ ) * _size_ * sizeof( _type_ ) ) == 0 ); }

		MatBase & operator+=( const MatBase & rhs );
		MatBase & operator-=( const MatBase & rhs );
		MatBase & operator*=( const MatBase & rhs );
		MatBase & operator*=( const float rhs );

		void Transpose();
		_matrix_ Transposed() const;

		Vec< _type_, 3 > Forward() const { return static_cast< Vec< _type_, 3 > >( CRTP().m_rows[ 2 ] ).Normalized(); }
		Vec< _type_, 3 > Right() const { return static_cast< Vec< _type_, 3 > >( CRTP().m_rows[ 0 ] ).Normalized(); }
		Vec< _type_, 3 > Up() const { return static_cast< Vec< _type_, 3 > >( CRTP().m_rows[ 1 ] ).Normalized(); }

		const Vec< _type_, _size_ > & Row( int rowIndex ) const { return CRTP().m_rows[ rowIndex ]; }
		Vec< _type_, _size_ > Column( int columnIndex ) const;

		operator _matrix_ & ( ) { return CRTP(); }
		operator const _matrix_ & ( ) const { return CRTP(); }

		_type_ * Data() const { return CRTP().m_data; }

		_matrix_ & CRTP() { return static_cast< _matrix_ & >( *this ); }
		const _matrix_ & CRTP() const { return static_cast< const _matrix_ & >( *this ); }
	};

	template< typename _type_, typename _matrix_, int _size_ >
	Vec< _type_, _size_ > & MatBase< _type_, _matrix_, _size_ >::operator[]( int rowIndex ) {
		QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < _size_ ) );
		return CRTP().m_rows[ rowIndex ];
	}

	template< typename _type_, typename _matrix_, int _size_ >
	const Vec< _type_, _size_ > & MatBase< _type_, _matrix_, _size_ >::operator[]( int rowIndex ) const {
		QP_ASSERT( ( rowIndex >= 0 ) && ( rowIndex < _size_ ) );
		return CRTP().m_rows[ rowIndex ];
	}

	template< typename _type_, typename _matrix_, int _size_ >
	_type_ & MatBase< _type_, _matrix_, _size_ >::operator()( int row, int column ) {
		QP_ASSERT( ( row > 0 ) && ( row <= _size_ ) );
		QP_ASSERT( ( column > 0 ) && ( column <= _size_ ) );
		return CRTP().m_data[ ( row - 1 ) * _size_ + ( column - 1 ) ];
	}

	template< typename _type_, typename _matrix_, int _size_ >
	const _type_ & MatBase< _type_, _matrix_, _size_ >::operator()( int row, int column ) const {
		QP_ASSERT( ( row > 0 ) && ( row <= _size_ ) );
		QP_ASSERT( ( column > 0 ) && ( column <= _size_ ) );
		return CRTP().m_data[ ( row - 1 ) * _size_ + ( column - 1 ) ];
	}

	template< typename _type_, typename _matrix_, int _size_ >
	MatBase< _type_, _matrix_, _size_ > & MatBase< _type_, _matrix_, _size_ >::operator+=( const MatBase & rhs ) {
		for ( int index = 0; index < NumElements(); ++index ) {
			CRTP().m_data[ index ] += rhs.CRTP().m_data[ index ];
		}
		return *this;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	MatBase< _type_, _matrix_, _size_ > & MatBase< _type_, _matrix_, _size_ >::operator-=( const MatBase & rhs ) {
		for ( int index = 0; index < NumElements(); ++index ) {
			CRTP().m_data[ index ] -= rhs.CRTP().m_data[ index ];
		}
		return *this;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	MatBase< _type_, _matrix_, _size_ > & MatBase< _type_, _matrix_, _size_ >::operator*=( const MatBase & rhs ) {
		for ( int columnIndex = 0; columnIndex < _size_; ++columnIndex ) {
			for ( int rowIndex = 0; rowIndex < _size_; ++rowIndex ) {
				CRTP().m_rows[ rowIndex ][ columnIndex ] = Dot( Row( rowIndex ), rhs.CRTP().Column( columnIndex ) );
			}
		}
		return *this;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	MatBase< _type_, _matrix_, _size_ > & MatBase< _type_, _matrix_, _size_ >::operator*=( const float rhs ) {
		for ( int index = 0; index < NumElements(); ++index ) {
			CRTP().m_data[ index ] *= rhs;
		}
		return *this;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	void MatBase< _type_, _matrix_, _size_ >::Transpose() {
		for ( int rowIndex = 0; rowIndex < _size_; ++rowIndex ) {
			for ( int columnIndex = 0; columnIndex < _size_; ++columnIndex ) {
				if ( rowIndex == columnIndex ) {
					continue;
				}
				Swap( CRTP().m_data[ rowIndex * _size_ + columnIndex ], CRTP().m_data[ columnIndex * _size_ + rowIndex ] );
			}
		}
	}

	template< typename _type_, typename _matrix_, int _size_ >
	_matrix_ MatBase< _type_, _matrix_, _size_ >::Transposed() const {
		_matrix_ transposed = *this;
		transposed.Transpose();
		return transposed;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	Vec< _type_, _size_ > MatBase< _type_, _matrix_, _size_ >::Column( int columnIndex ) const {
		Vec< _type_, _size_ > column;
		for ( int rowIndex = 0; rowIndex < _size_; ++rowIndex ) {
			column[ rowIndex ] = CRTP().m_rows[ rowIndex ][ columnIndex ];
		}
		return column;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	inline _matrix_ operator+( const MatBase< _type_, _matrix_, _size_ > & lhs, const MatBase< _type_, _matrix_, _size_ > & rhs ) {
		_matrix_ result( lhs );
		result += rhs;
		return result;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	inline _matrix_ operator-( const MatBase< _type_, _matrix_, _size_ > & lhs, const MatBase< _type_, _matrix_, _size_ > & rhs ) {
		_matrix_ result( lhs );
		result -= rhs;
		return result;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	inline _matrix_ operator*( const MatBase< _type_, _matrix_, _size_ > & lhs, const MatBase< _type_, _matrix_, _size_ > & rhs ) {
		_matrix_ result( lhs );
		result *= rhs;
		return result;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	inline _matrix_ operator*( const MatBase< _type_, _matrix_, _size_ > & lhs, const float rhs ) {
		_matrix_ result( lhs );
		result *= rhs;
		return result;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	inline _matrix_ operator*( const float lhs, const MatBase< _type_, _matrix_, _size_ > & rhs ) {
		_matrix_ result( rhs );
		result *= lhs;
		return result;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	inline Vec< _type_, _size_ > operator*( const Vec< _type_, _size_ > & lhs, const MatBase< _type_, _matrix_, _size_ > & rhs ) {
		Vec< _type_, _size_ > result( lhs );
		for ( int columnIndex = 0; columnIndex < _size_; ++columnIndex ) {
			result[ columnIndex ] = Dot( lhs, rhs.CRTP().Column( columnIndex ) );
		}
		return result;
	}

	template < typename _type_, int _size_ >
	class Mat : public MatBase< _type_, Mat< _type_, _size_ >, _size_ > {
	public:
		union {
			_type_ m_data[ _size_ * _size_ ] {};
			Vec< _type_, _size_ > m_rows[ _size_ ];
		};
	};

	template < typename _type_ >
	class Mat< _type_, 3 > : public MatBase< _type_, Mat< _type_, 3 >, 3 > {
	public:
		Mat()
			: m_data {
				static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ),
				static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ),
				static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 )
			} {
		}
		Mat( const float m11, const float m12, const float m13,
				const float m21, const float m22, const float m23,
				const float m31, const float m32, const float m33 )
			: m_data {
				 m11, m12, m13,
				 m21, m22, m23,
				 m31, m32, m33
			} {
		}
		Mat( const Vec< _type_, 3 > rowA, const Vec< _type_, 3 > rowB, const Vec< _type_, 3 > rowC )
			: m_rows { rowA, rowB, rowC } {
		}
		union {
			_type_ m_data[ 3 * 3 ] {};
			Vec< _type_, 3 > m_rows[ 3 ];
		};
	};

	template < typename _type_ >
	class Mat< _type_, 4 > : public MatBase< _type_, Mat< _type_, 4 >, 4 > {
	public:
		Mat()
			: m_data {
				static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ),
				static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ),
				static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ), static_cast< _type_ >( 0 ),
				static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 )
			} {
		}
		Mat( const float m11, const float m12, const float m13, const float m14,
			   const float m21, const float m22, const float m23, const float m24,
			   const float m31, const float m32, const float m33, const float m34,
			   const float m41, const float m42, const float m43, const float m44 )
			: m_data {
				 m11, m12, m13, m14,
				 m21, m22, m23, m24,
				 m31, m32, m33, m34,
				 m41, m42, m43, m44
			} {
		}
		Mat( const Vec< _type_, 4 > rowA, const Vec< _type_, 4 > rowB, const Vec< _type_, 4 > rowC, const Vec< _type_, 4 > rowD )
			: m_rows { rowA, rowB, rowC, rowD } {
		}
		union {
			_type_ m_data[ 4 * 4 ] {};
			Vec< _type_, 4 > m_rows[ 4 ];
		};

		Vec< _type_, 3 > TransformPoint( const Vec< _type_, 3 > & point );
		Vec< _type_, 3 > TransformPoint( const Vec< _type_, 4 > & point );
		Vec< _type_, 3 > TransformVector( const Vec< _type_, 3 > & vector );
		Vec< _type_, 3 > TransformVector( const Vec< _type_, 4 > & vector );
	};

	template< typename _type_ >
	Vec< _type_, 3 > Mat< _type_, 4 >::TransformPoint( const Vec< _type_, 3 > & point ) {
		return static_cast< Vec< _type_, 3 > >( operator*( Vec< _type_, 4 >( point, static_cast< _type_ >( 1 ) ), *this ) );
	}

	template< typename _type_ >
	Vec< _type_, 3 > Mat< _type_, 4 >::TransformPoint( const Vec< _type_, 4 > & point ) {
		return static_cast< Vec< _type_, 3 > >( operator*( Vec< _type_, 4 >( point.x, point.y, point.z, static_cast< _type_ >( 1 ) ), *this ) );
	}

	template< typename _type_ >
	Vec< _type_, 3 > Mat< _type_, 4 >::TransformVector( const Vec< _type_, 3 > & vector ) {
		return static_cast< Vec< _type_, 3 > >( operator*( Vec< _type_, 4 >( vector, static_cast< _type_ >( 0 ) ), *this ) );
	}

	template< typename _type_ >
	Vec< _type_, 3 > Mat< _type_, 4 >::TransformVector( const Vec< _type_, 4 > & vector ) {
		return static_cast< Vec< _type_, 3 > >( operator*( Vec< _type_, 4 >( vector.x, vector.y, vector.z, static_cast< _type_ >( 0 ) ), *this ) );
	}
	
	template< typename _type_ >
	Mat< _type_, 4 > PerspectiveProjectionMatrix( const _type_ & hFovDeg, const int width, const int height, const _type_ & nearClip, const _type_ & farClip ) {
		const _type_ hFovRad = math::Deg2Rad( hFovDeg );
		const _type_ vFovRad = static_cast< _type_ >( 2.0 ) * math::Atan< _type_ >( math::Tan< _type_ >( hFovRad / static_cast< _type_ >( 2.0 ) ) );

		const _type_ aspectRatio = static_cast< _type_ >( height ) / static_cast< _type_ >( width );

		const _type_ A = farClip / ( farClip - nearClip );
		Mat< _type_, 4 > projectionMatrix;
		projectionMatrix( 1, 1 ) = static_cast< _type_ >( 1.0 ) / math::Tan< _type_ >( hFovRad * static_cast< _type_ >( 0.5 ) );
		projectionMatrix( 2, 2 ) = static_cast< _type_ >( -1.0 ) / math::Tan< _type_ >( vFovRad * static_cast< _type_ >( 0.5 ) * aspectRatio );
		projectionMatrix( 3, 3 ) = A;
		projectionMatrix( 3, 4 ) = static_cast< _type_ >( 1.0 ) / A;
		projectionMatrix( 4, 3 ) = A * -nearClip;
		projectionMatrix( 4, 4 ) = static_cast< _type_ >( 0.0 );
		return projectionMatrix;
	}
	template< typename _type_ >
	Mat< _type_, 4 > OrthographicProjectionMatrix( const _type_ & left, const _type_ & right, const _type_ & bottom, const _type_ & top, const _type_ & near, const _type_ & far ) {
		Mat< _type_, 4 > projection;
		projection( 1, 1 ) = static_cast< _type_ >( 2.0 ) / ( right - left );
		projection( 2, 2 ) = static_cast< _type_ >( -2.0 ) / ( top - bottom );
		projection( 3, 3 ) = static_cast< _type_ >( 1.0 ) / ( far - near );
		projection( 4, 1 ) = static_cast< _type_ >( -( ( right + left ) / ( right - left ) ) );
		projection( 4, 2 ) = static_cast< _type_ >( -( ( top + bottom ) / ( top - bottom ) ) );
		projection( 4, 3 ) = static_cast< _type_ >( -( near / ( near - far ) ) );
		projection( 4, 4 ) = static_cast< _type_ >( 1 );
		return projection;
	}
	template< typename _type_ >
	Mat< _type_, 4 > AffineInverse( const Mat< _type_, 4 > & matrix ) {
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

		Mat< _type_, 4 > inverse;

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

		const Vec< _type_, 4 > & translation = -matrix[ 3 ];
		inverse[ 3 ] = Vec< _type_, 4 >( inverse.TransformPoint( translation ), static_cast< _type_ >( 1.0 ) );

		return inverse;
	}

	template< typename _type_ >
	Mat< _type_, 4 > RotationAndTranslationInverse( const Mat< _type_, 4 > & matrix ) {
		Mat< _type_, 4 > inverse;
		inverse( 1, 1 ) = matrix( 1, 1 );
		inverse( 1, 2 ) = matrix( 2, 1 );
		inverse( 1, 3 ) = matrix( 3, 1 );

		inverse( 2, 1 ) = matrix( 1, 2 );
		inverse( 2, 2 ) = matrix( 2, 2 );
		inverse( 2, 3 ) = matrix( 3, 2 );

		inverse( 3, 1 ) = matrix( 1, 3 );
		inverse( 3, 2 ) = matrix( 2, 3 );
		inverse( 3, 3 ) = matrix( 3, 3 );

		inverse[ 3 ] = Vec< _type_, 4 >( inverse.TransformPoint( static_cast< Vec< _type_, 3 > >( -matrix[ 3 ] ) ), static_cast< _type_ >( 1.0 ) );

		return inverse;
	}

	template< typename _type_ >
	Mat< _type_, 4 > CreateTranslation( const Vec< _type_, 3 > & translation ) {
		Mat< _type_, 4 > translationMatrix;
		translationMatrix( 4, 1 ) = translation.x;
		translationMatrix( 4, 2 ) = translation.y;
		translationMatrix( 4, 3 ) = translation.z;
		return translationMatrix;
	}

	template< typename _type_ >
	Mat< _type_, 4 > CreateScale( const Vec< _type_, 3 > & scale ) {
		Mat< _type_, 4 > scaleMatrix;
		scaleMatrix( 1, 1 ) = scale.x;
		scaleMatrix( 2, 2 ) = scale.y;
		scaleMatrix( 3, 3 ) = scale.z;
		return scaleMatrix;
	}


	template< typename _type_ >
	Mat< _type_, 4 > CreateRotationX( const _type_ angleDeg ) {
		const _type_ angleRad = math::Deg2Rad( angleDeg );
		Mat< _type_, 4 > rotationMatrix;
		rotationMatrix( 2, 2 ) = math::Cos( angleRad );
		rotationMatrix( 2, 3 ) = math::Sin( angleRad );
		rotationMatrix( 3, 3 ) = math::Cos( angleRad );
		rotationMatrix( 3, 2 ) = -math::Sin( angleRad );
		return rotationMatrix;
	}

	template< typename _type_ >
	Mat< _type_, 4 > CreateRotationY( const _type_ angleDeg ) {
		const _type_ angleRad = math::Deg2Rad( angleDeg );
		Mat< _type_, 4 > rotationMatrix;
		rotationMatrix( 1, 1 ) = math::Cos( angleRad );
		rotationMatrix( 1, 3 ) = -math::Sin( angleRad );
		rotationMatrix( 3, 1 ) = math::Sin( angleRad );
		rotationMatrix( 3, 3 ) = math::Cos( angleRad );
		return rotationMatrix;
	}

	template< typename _type_ >
	Mat< _type_, 4 > CreateRotationZ( const _type_ angleDeg ) {
		const _type_ angleRad = math::Deg2Rad( angleDeg );
		Mat< _type_, 4 > rotationMatrix;
		rotationMatrix( 1, 1 ) = math::Cos( angleRad );
		rotationMatrix( 1, 2 ) = math::Sin( angleRad );
		rotationMatrix( 2, 1 ) = -math::Sin( angleRad );
		rotationMatrix( 2, 2 ) = math::Cos( angleRad );
		return rotationMatrix;
	}

	template< typename _type_, typename _matrix_, int _size_ >
	_matrix_ Transpose( const MatBase< _type_, _matrix_, _size_ > & matrix ) {
		return matrix.Transposed();
	}

	using Mat3 = Mat< float, 3 >;
	using Mat3d = Mat< double, 3 >;

	using Mat4 = Mat< float, 4 >;
	using Mat4d = Mat< double, 4 >;

	static const Mat3 g_mat3Identity {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	static const Mat4 g_mat4Identity {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	static const Mat4 g_mat4NaN {
		NAN, NAN, NAN, NAN,
		NAN, NAN, NAN, NAN,
		NAN, NAN, NAN, NAN,
		NAN, NAN, NAN, NAN
	};
}