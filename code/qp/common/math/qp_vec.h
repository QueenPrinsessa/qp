#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/math/qp_math.h"

namespace qp {
	// todo: implement vector intrinsics support
	//#define QP_USE_VECTOR_INTRINSICS 
#define QP_ASSERT_NORMALIZE_ZERO_VECTOR
	template < typename _type_, typename _vector_ >
	class VecBase {
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
	_type_ VecBase<_type_, _vector_>::LengthSqr() const {
		_type_ length = 0;
		for ( int index = 0; index < NumElements(); index++ ) {
			length += math::Pow2( CRTP().m_data[ index ] );
		}
		return length;
	}

	template< typename _type_, typename _vector_ >
	_type_ VecBase<_type_, _vector_>::Length() const {
		return math::Sqrt( LengthSqr() );
	}

	template< typename _type_, typename _vector_ >
	void VecBase< _type_, _vector_ >::Normalize() requires IsFloatingPoint< _type_ > {
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
	_vector_ VecBase< _type_, _vector_ >::Normalized() requires IsFloatingPoint< _type_ > {
		_vector_ result = CRTP();
		result.Normalize();
		return result;
	}

	template< typename _type_, typename _vector_ >
	void VecBase<_type_, _vector_>::Zero() {
		ZeroMemory( &CRTP().m_data[ 0 ], sizeof( _type_ ) * NumElements() );
	}

	template< typename _type_, typename _vector_ >
	_vector_ VecBase< _type_, _vector_ >::operator-() const {
		_vector_ result;
		for ( int index = 0; index < NumElements(); ++index ) {
			result[ index ] = -CRTP().m_data[ index ];
		}
		return result;
	}

	template< typename _type_, typename _vector_ >
	inline _vector_ operator-=( VecBase< _type_, _vector_ > & lhs, const VecBase< _type_, _vector_ > & rhs ) {
		for ( int index = 0; index < lhs.NumElements(); ++index ) {
			lhs[ index ] -= rhs[ index ];
		}
		return lhs.CRTP();
	}

	template< typename _type_, typename _vector_ >
	inline _vector_ operator-( const VecBase< _type_, _vector_ > & lhs, const VecBase< _type_, _vector_ > & rhs ) {
		_vector_ result( lhs.CRTP() );
		result -= rhs;
		return result;
	}

	template< typename _type_, typename _vector_ >
	inline _vector_ operator+=( VecBase< _type_, _vector_ > & lhs, const VecBase< _type_, _vector_ > & rhs ) {
		for ( int index = 0; index < lhs.NumElements(); ++index ) {
			lhs[ index ] += rhs[ index ];
		}
		return lhs.CRTP();
	}

	template< typename _type_, typename _vector_ >
	inline _vector_ operator+( const VecBase< _type_, _vector_ > & lhs, const VecBase< _type_, _vector_ > & rhs ) {
		_vector_ result( lhs.CRTP() );
		result += rhs;
		return result;
	}

	template< typename _type_, typename _vector_ >
	inline _vector_ operator*=( VecBase< _type_, _vector_ > & lhs, const _type_ & rhs ) {
		for ( int index = 0; index < lhs.NumElements(); ++index ) {
			lhs[ index ] *= rhs;
		}
		return lhs.CRTP();
	}

	template< typename _type_, typename _vector_ >
	inline _vector_ operator*( const VecBase< _type_, _vector_ > & lhs, const _type_ & rhs ) {
		_vector_ result( lhs.CRTP() );
		result *= rhs;
		return result;
	}

	template < typename _type_, int _length_ >
	class Vec : public VecBase< _type_, Vec< _type_, _length_ > > {
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
	class Vec< _type_, 2 > : public VecBase< _type_, Vec< _type_, 2 > > {
	public:
		Vec()
			: Vec( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ) ) {
		}
		Vec( _type_ n )
			: Vec( n, n ) {
		}
		Vec( _type_ _x, _type_ _y )
			: m_data { _x, _y } {
		}
		explicit Vec( const Vec< _type_, 3 > & vec )
			: Vec( vec.x, vec.y ) {
		}
		union {
			_type_ m_data[ 2 ] {};
			struct {
				_type_ x;
				_type_ y;
			};
		};
	};

	template < typename _type_ >
	class Vec< _type_, 3 > : public VecBase< _type_, Vec< _type_, 3 > > {
	public:
		Vec()
			: Vec( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ) ) {
		}
		Vec( _type_ n )
			: Vec( n, n, n ) {
		}
		Vec( _type_ _x, _type_ _y, _type_ _z )
			: m_data { _x, _y, _z } {
		}
		explicit Vec( const Vec< _type_, 4 > & vec )
			: Vec( vec.x, vec.y, vec.z ) {
		}
		explicit Vec( const Vec< _type_, 2 > & vec, const _type_ & _z )
			: Vec( vec.x, vec.y, _z ) {
		}
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
	class Vec< _type_, 4 > : public VecBase< _type_, Vec< _type_, 4 > > {
	public:
		Vec()
			: Vec( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ) ) {
		}
		Vec( _type_ n )
			: Vec( n, n, n, n ) {
		}
		Vec( _type_ _x, _type_ _y, _type_ _z, _type_ _w )
			: m_data { _x, _y, _z, _w } {
		}
		explicit Vec( const Vec< _type_, 3 > & vec, const _type_ & _w )
			: Vec( vec.x, vec.y, vec.z, _w ) {
		}
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
	_type_ Dot( const VecBase< _type_, _vector_ > & lhs, const VecBase< _type_, _vector_ > & rhs ) {
		_type_ dot = static_cast< _type_ >( 0 );
		for ( int index = 0; index < lhs.NumElements(); ++index ) {
			dot += lhs.CRTP().m_data[ index ] * rhs.CRTP().m_data[ index ];
		}
		return dot;
	}

	template < typename _type_ >
	Vec< _type_, 3 > Cross( const Vec< _type_, 3 > & lhs, const Vec< _type_, 3 > & rhs ) {
		return Vec< _type_, 3 >( ( ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ) ),
							  ( ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ) ),
							  ( ( lhs.x * rhs.y ) - ( lhs.y * rhs.x ) ) );
	}

	using Vec2 = Vec< float, 2 >;
	using Vec2d = Vec< double, 2 >;
	using Vec2i = Vec< int, 2 >;
	using Vec2ui = Vec< unsigned int, 2 >;

	using Vec3 = Vec< float, 3 >;
	using Vec3d = Vec< double, 3 >;
	using Vec3i = Vec< int, 3 >;
	using Vec3ui = Vec< unsigned int, 3 >;

	using Vec4 = Vec< float, 4 >;
	using Vec4d = Vec< double, 4 >;
	using Vec4i = Vec< int, 4 >;
	using Vec4ui = Vec< unsigned int, 4 >;

	static const Vec2 g_vec2Zero { 0.0f };
	static const Vec3 g_vec3Zero { 0.0f };
	static const Vec4 g_vec4Zero { 0.0f };

	static const Vec3 g_vec3Right { 1.0f, 0.0f, 0.0f };
	static const Vec3 g_vec3Up { 0.0f, 1.0f, 0.0f };
	static const Vec3 g_vec3Forward { 0.0f, 0.0f, 1.0f };
}