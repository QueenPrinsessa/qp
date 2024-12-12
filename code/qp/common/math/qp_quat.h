#pragma once
#include "qp_mat.h"
#include "qp_math.h"
#include "qp_vec.h"

namespace qp {
	template < typename _type_ >
	class Quaternion {
	public:
		_type_ x = static_cast< _type_ >( 0 );
		_type_ y = static_cast< _type_ >( 0 );
		_type_ z = static_cast< _type_ >( 0 );
		_type_ w = static_cast< _type_ >( 1 );

		Quaternion() = default;
		Quaternion( const _type_ & _x, const _type_ & _y, const _type_ & _z, const _type_ & _w );
		Quaternion( const _type_ & xDeg, const _type_ & yDeg, const _type_ & zDeg );
		Quaternion( const Vec< _type_, 3 > & axis, const _type_ & angleDeg );
		explicit Quaternion( const Mat< _type_, 3 > & mat );
		explicit Quaternion( const Mat< _type_, 4 > & mat );

		_type_ Length() const;
		_type_ LengthSqr() const;

		Quaternion Normalized() const;
		void Normalize();

		Vec< _type_, 3 > ToEulers() const;

		Vec< _type_, 3 > Forward() const;
		Vec< _type_, 3 > Right() const;
		Vec< _type_, 3 > Up() const;

		Mat< _type_, 3 > ToMat3() const;
		Mat< _type_, 4 > ToMat4() const;

		Quaternion operator-() const;

		Vec< _type_, 3 > Axis() const;

		_type_ * Data() { return &x; }
		const _type_ * Data() const { return &x; }
	private:
	};

	template< typename _type_ >
	Quaternion< _type_ >::Quaternion( const _type_ & _x, const _type_ & _y, const _type_ & _z, const _type_ & _w )
		: x( _x ), y( _y ), z( _z ), w( _w ) {
	}

	template< typename _type_ >
	Quaternion< _type_ >::Quaternion( const _type_ & xDeg, const _type_ & yDeg, const _type_ & zDeg ) {
		Vec< _type_, 3 > halfAngleRad( math::Deg2Rad( xDeg * static_cast< _type_ >( 0.5 ) ),
									 math::Deg2Rad( yDeg * static_cast< _type_ >( 0.5 ) ),
									 math::Deg2Rad( zDeg * static_cast< _type_ >( 0.5 ) ) );

		const Quaternion< _type_ > pitch { math::Sin( halfAngleRad.x ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), math::Cos( halfAngleRad.x ) };
		const Quaternion< _type_ > yaw { static_cast< _type_ >( 0 ), math::Sin( halfAngleRad.y ), static_cast< _type_ >( 0 ), math::Cos( halfAngleRad.y ) };
		const Quaternion< _type_ > roll { static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), math::Sin( halfAngleRad.z ), math::Cos( halfAngleRad.z ) };

		Quaternion< _type_ > result = roll * yaw * pitch;
		result.Normalize();
		x = result.x;
		y = result.y;
		z = result.z;
		w = result.w;
	}

	template< typename _type_ >
	Quaternion<  _type_  >::Quaternion( const Vec<_type_, 3> & axis, const _type_ & angleDeg ) {
		_type_ halfAngleRad = math::Deg2Rad( angleDeg * static_cast< _type_ >( 0.5 ) );
		_type_ sinHalfAngle = math::Sin( halfAngleRad );
		x = axis.x * sinHalfAngle;
		y = axis.y * sinHalfAngle;
		z = axis.z * sinHalfAngle;
		w = math::Cos( halfAngleRad );
	}

	//From: https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
	template< typename _type_ >
	Quaternion<  _type_  >::Quaternion( const Mat<_type_, 3> & mat ) {
		const _type_ m00 = mat( 1, 1 );
		const _type_ m01 = mat( 2, 1 );
		const _type_ m02 = mat( 3, 1 );

		const _type_ m10 = mat( 1, 2 );
		const _type_ m11 = mat( 2, 2 );
		const _type_ m12 = mat( 3, 2 );

		const _type_ m20 = mat( 1, 3 );
		const _type_ m21 = mat( 2, 3 );
		const _type_ m22 = mat( 3, 3 );

		Quaternion< _type_ > quatFromMatrix;

		_type_ t = static_cast< _type_ >( 0 );
		if ( m22 < static_cast< _type_ >( 0 ) ) {
			if ( m00 > m11 ) {
				t = static_cast< _type_ >( 1 ) + m00 - m11 - m22;

				quatFromMatrix.x = t;
				quatFromMatrix.y = m01 + m10;
				quatFromMatrix.z = m20 + m02;
				quatFromMatrix.w = m12 - m21;
			} else {
				t = static_cast< _type_ >( 1 ) - m00 + m11 - m22;
				quatFromMatrix.x = m01 + m10;
				quatFromMatrix.y = t;
				quatFromMatrix.z = m12 + m21;
				quatFromMatrix.w = m20 - m02;
			}
		} else {
			if ( m00 < -m11 ) {
				t = static_cast< _type_ >( 1 ) - m00 - m11 + m22;
				quatFromMatrix.x = m20 + m02;
				quatFromMatrix.y = m12 + m21;
				quatFromMatrix.z = t;
				quatFromMatrix.w = m01 - m10;
			} else {
				t = static_cast< _type_ >( 1 ) + m00 + m11 + m22;
				quatFromMatrix.x = m12 - m21;
				quatFromMatrix.y = m20 - m02;
				quatFromMatrix.z = m01 - m10;
				quatFromMatrix.w = t;
			}
		}

		quatFromMatrix *= static_cast< _type_ >( 0.5 ) / math::Sqrt( t );
		quatFromMatrix.w *= static_cast< _type_ >( -1 );
		quatFromMatrix.Normalize();

		x = quatFromMatrix.x;
		y = quatFromMatrix.y;
		z = quatFromMatrix.z;
		w = quatFromMatrix.w;
	}

	template< typename _type_ >
	Quaternion<  _type_  >::Quaternion( const Mat<_type_, 4> & mat )
		: Quaternion( static_cast< Vec< _type_, 3 > >( mat.Right() ), static_cast< Vec< _type_, 3 > >( mat.Up() ), static_cast< Vec< _type_, 3 > >( mat.Forward() ) ) {
	}

	template< typename _type_ >
	_type_ Quaternion<  _type_  >::Length() const {
		return math::Sqrt( LengthSqr() );
	}

	template< typename _type_ >
	_type_ Quaternion<  _type_  >::LengthSqr() const {
		return Dot( *this, *this );
	}

	template< typename _type_ >
	Quaternion<  _type_  > Quaternion<  _type_  >::Normalized() const {
		Quaternion< _type_ > result;
		result.Normalize();
		return result;
	}

	template< typename _type_ >
	void Quaternion<  _type_  >::Normalize() {
		_type_ magnitude = Length();

		if ( magnitude != static_cast< _type_ >( 0 ) ) {
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
			w /= magnitude;
		}
	}

	template< typename _type_ >
	Vec<_type_, 3> Quaternion<  _type_  >::ToEulers() const {
		Vec< _type_, 3 > angles( 0.0f );

		// x-axis rotation
		_type_ sinXCosY = static_cast< _type_ >( 2 ) * ( w * x + y * z );
		_type_ cosXSinY = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + y * y );
		angles.x = math::Atan2( sinXCosY, cosXSinY );

		// y-axis rotation
		_type_ sinY = static_cast< _type_ >( 2 ) * ( w * y - z * x );
		if ( math::Abs( sinY ) >= static_cast< _type_ >( 1 ) ) {
			angles.y = math::CopySign( math::Pi / 2, sinY ); // use 90 degrees if out of range
		} else {
			angles.y = math::Asin( sinY );
		}

		// z-axis rotation
		_type_ sinZCosY = static_cast< _type_ >( 2 ) * ( w * z + x * y );
		_type_ cosZCosY = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( y * y + z * z );
		angles.z = math::Atan2( sinZCosY, cosZCosY );

		return angles;
	}

	template< typename _type_ >
	Vec<_type_, 3> Quaternion<  _type_  >::Forward() const {
		Vec< _type_, 3 > forward;
		forward.x = static_cast< _type_ >( 2 ) * ( x * z + w * y );
		forward.y = static_cast< _type_ >( 2 ) * ( y * z - w * x );
		forward.z = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + y * y );
		return forward;
	}

	template< typename _type_ >
	Vec<_type_, 3> Quaternion<  _type_  >::Right() const {
		Vec< _type_, 3 > right;
		right.x = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( y * y + z * z );
		right.y = static_cast< _type_ >( 2 ) * ( x * y + w * z );
		right.z = static_cast< _type_ >( 2 ) * ( x * z - w * y );
		return right;
	}

	template< typename _type_ >
	Vec<_type_, 3> Quaternion<  _type_  >::Up() const {
		Vec< _type_, 3 > up;
		up.x = static_cast< _type_ >( 2 ) * ( x * y - w * z );
		up.y = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + z * z );
		up.z = static_cast< _type_ >( 2 ) * ( y * z + w * x );
		return up;
	}

	template< typename _type_ >
	Mat<_type_, 3> Quaternion<  _type_  >::ToMat3() const {
		Mat< _type_, 3 > rotationMatrix;

		rotationMatrix( 1, 1 ) = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( y * y + z * z );
		rotationMatrix( 1, 2 ) = static_cast< _type_ >( 2 ) * ( x * y + w * z );
		rotationMatrix( 1, 3 ) = static_cast< _type_ >( 2 ) * ( x * z - w * y );

		rotationMatrix( 2, 1 ) = static_cast< _type_ >( 2 ) * ( x * y - w * z );
		rotationMatrix( 2, 2 ) = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + z * z );
		rotationMatrix( 2, 3 ) = static_cast< _type_ >( 2 ) * ( y * z + w * x );

		rotationMatrix( 3, 1 ) = static_cast< _type_ >( 2 ) * ( x * z + w * y );
		rotationMatrix( 3, 2 ) = static_cast< _type_ >( 2 ) * ( y * z - w * x );
		rotationMatrix( 3, 3 ) = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + y * y );

		return rotationMatrix;
	}

	template< typename _type_ >
	Mat<_type_, 4> Quaternion<  _type_  >::ToMat4() const {
		const Mat< _type_, 3 > rotationMatrix = ToMat3();
		return Mat< _type_, 4 > {
			Vec< _type_, 4 >( rotationMatrix.Right(), static_cast< _type_ >( 0 ) ),
				Vec< _type_, 4 >( rotationMatrix.Up(), static_cast< _type_ >( 0 ) ),
				Vec< _type_, 4 >( rotationMatrix.Forward(), static_cast< _type_ >( 0 ) ),
				Vec< _type_, 4 >( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ) ),
		};
	}

	template< typename _type_ >
	Quaternion<  _type_  > Quaternion<  _type_  >::operator-() const {
		return qpQuaternion< _type_ >( -x, -y, -z, -w );
	}

	template< typename _type_ >
	Vec<_type_, 3> Quaternion<  _type_  >::Axis() const {
		return Vec< _type_, 3 >( x, y, z );
	}

	template < typename _type_ >
	static Quaternion< _type_ > & operator*=( Quaternion< _type_ > & lhs, const Quaternion< _type_ > & rhs ) {
		_type_ x = lhs.x;
		_type_ y = lhs.y;
		_type_ z = lhs.z;
		_type_ w = lhs.w;

		lhs.x = ( w * rhs.x ) + ( x * rhs.w ) + ( y * rhs.z ) - ( z * rhs.y );
		lhs.y = ( w * rhs.y ) - ( x * rhs.z ) + ( y * rhs.w ) + ( z * rhs.x );
		lhs.z = ( w * rhs.z ) + ( x * rhs.y ) - ( y * rhs.x ) + ( z * rhs.w );
		lhs.w = ( w * rhs.w ) - ( x * rhs.x ) - ( y * rhs.y ) - ( z * rhs.z );

		return lhs;
	}

	template < typename _type_ >
	static Quaternion< _type_ > & operator*=( Quaternion< _type_ > & lhs, const _type_ & rhs ) {
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		lhs.z *= rhs.z;
		lhs.w *= rhs.w;
		return lhs;
	}

	template < typename _type_ >
	static Quaternion< _type_ > operator*( const Quaternion< _type_ > & lhs, const Quaternion< _type_ > & rhs ) {
		Quaternion< _type_ > result = lhs;
		result *= rhs;
		return result;
	}

	template < typename _type_ >
	static Quaternion< _type_ > operator*( const Quaternion< _type_ > & lhs, const _type_ & rhs ) {
		Quaternion< _type_ > result = lhs;
		result *= rhs;
		return result;
	}

	template < typename _type_ >
	static Quaternion< _type_ > operator*( const _type_ & lhs, const Quaternion< _type_ > & rhs ) {
		Quaternion< _type_ > result = rhs;
		result *= lhs;
		return result;
	}

	template < typename _type_ >
	static Quaternion< _type_ > & operator/=( Quaternion< _type_ > & lhs, const _type_ & rhs ) {
		lhs.x /= rhs.x;
		lhs.y /= rhs.y;
		lhs.z /= rhs.z;
		lhs.w /= rhs.w;
		return lhs;
	}

	template < typename _type_ >
	static Quaternion< _type_ > operator/( const Quaternion< _type_ > & lhs, const _type_ & rhs ) {
		Quaternion< _type_ > result = lhs;
		result /= rhs;
		return result;
	}

	template < typename _type_ >
	static Quaternion< _type_ > operator/( const _type_ & lhs, const Quaternion< _type_ > & rhs ) {
		Quaternion< _type_ > result = rhs;
		result /= lhs;
		return result;
	}

	template < typename _type_ >
	static Quaternion< _type_ > & operator+=( Quaternion< _type_ > & lhs, const Quaternion< _type_ > & rhs ) {
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		lhs.w += rhs.w;
		return lhs;
	}

	template < typename _type_ >
	static Quaternion< _type_ > operator+( const Quaternion< _type_ > & lhs, const Quaternion< _type_ > & rhs ) {
		Quaternion< _type_ > result = lhs;
		result += rhs;
		return result;
	}

	template < typename _type_ >
	static Quaternion< _type_ > Conjugate( const Quaternion< _type_ > & quat ) {
		return Quaternion< _type_ >( -quat.x, -quat.y, -quat.z, quat.w );
	}

	template < typename _type_ >
	static Quaternion< _type_ > LookRotation( const Vec< _type_, 3 > & forward, const Vec< _type_, 3 > & up ) {
		Vec< _type_, 3 > lookRight = qpCross( up, forward );
		Vec< _type_, 3 > lookUp = qpCross( forward, lookRight );
		return Quaternion< _type_ >( Mat< _type_, 3 >( lookRight, lookUp, forward ) );
	}

	template < typename _type_ >
	static Quaternion< _type_ > LookRotation( const Vec< _type_, 3 > & forward ) {
		return LookRotation( forward, g_vec3Up );
	}

	template < typename _type_ >
	static Quaternion< _type_ > FromMat3( const Mat< _type_, 3 > & mat ) {
		return Quaternion< _type_ >( mat );
	}

	template < typename _type_ >
	static Quaternion< _type_ > FromMat4( const Mat< _type_, 4 > & mat ) {
		return Quaternion< _type_ >( mat );
	}

	template < typename _type_ >
	static Quaternion< _type_ > FromEulers( const _type_ & xDeg, const _type_ & yDeg, const _type_ & zDeg ) {
		return Quaternion< _type_ >( xDeg, yDeg, zDeg );
	}

	template < typename _type_ >
	static Quaternion< _type_ > FromEulers( const Vec< _type_, 3 > & eulersDeg ) {
		return FromEulers( eulersDeg.x, eulersDeg.y, eulersDeg.z );
	}

	template < typename _type_ >
	static Quaternion< _type_ > FromAxisAngle( const Vec< _type_, 3 > & axis, const _type_ & angleDeg ) {
		return Quaternion< _type_ >( axis, angleDeg );
	}

	template < typename _type_ >
	static Quaternion< _type_ > Lerp( const Quaternion< _type_ > & from, const Quaternion< _type_ > & to, const _type_ & time ) {
		return math::Lerp( from, to, time );
	}

	template < typename _type_ >
	static Quaternion< _type_ > Slerp( const Quaternion< _type_ > & from, const Quaternion< _type_ > & to, const _type_ & time ) {
		const Quaternion< _type_ > a = from;
		Quaternion< _type_ > b = to;

		_type_ dot = qpDot( a, b );

		if ( dot < static_cast< _type_ >( 0 ) ) {
			b = -b;
			dot *= static_cast< _type_ >( -1 );
		}

		_type_ dotABSqr = math::Min( dot * dot, static_cast< _type_ >( 1 ) );
		_type_ sinTheta = math::Sqrt( static_cast< _type_ >( 1 ) - dotABSqr );
		if ( sinTheta < static_cast< _type_ >( 0.0001 ) ) {
			return a;
		}

		return ( ( sinTheta * ( static_cast< _type_ >( 1 ) - time ) ) / sinTheta ) * a + ( ( sinTheta * time ) / sinTheta ) * b;
	}

	template < typename _type_ >
	static _type_ Dot( const Quaternion< _type_ > & lhs, const Quaternion< _type_ > & rhs ) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}

	template < typename _type_ >
	Quaternion< _type_ > DeltaRotationLocal( const Quaternion< _type_ > & from, const Quaternion< _type_ > & to ) {
		return  qpConjugate( from ) * to;
	}

	template < typename _type_ >
	Quaternion< _type_ > DeltaRotationWorld( const Quaternion< _type_ > & from, const Quaternion< _type_ > & to ) {
		return  to * qpConjugate( from );
	}

	using Quat = Quaternion< float >;
	using Quatd = Quaternion< double >;


	static const Quat g_quatIdentity { 0.0f, 0.0f, 0.0f, 1.0f };
	static const Quat g_quatUp { g_vec3Up, 1.0f };
	static const Quat g_quatForward { g_vec3Forward, 1.0f };
	static const Quat g_quatRight { g_vec3Right, 1.0f };
}