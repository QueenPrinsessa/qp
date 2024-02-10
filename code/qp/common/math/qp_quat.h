#pragma once
#include "qp_mat.h"
#include "qp_math.h"
#include "qp_vec.h"

template < typename T >
class qpQuaternion {
public:
	T x = static_cast< T >( 0 );
	T y = static_cast< T >( 0 );
	T z = static_cast< T >( 0 );
	T w = static_cast< T >( 1 );

	qpQuaternion() = default;
	qpQuaternion( const T & _x, const T & _y, const T & _z, const T & _w );
	qpQuaternion( const T & xDeg, const T & yDeg, const T & zDeg );
	qpQuaternion( const qpVec< T, 3 > & axis, const T & angleDeg );
	explicit qpQuaternion( const qpMat< T, 3 > & mat );
	explicit qpQuaternion( const qpMat< T, 4 > & mat );

	T Length() const;
	T LengthSqr() const;

	qpQuaternion Normalized() const;
	void Normalize();

	qpVec< T, 3 > ToEulers() const;

	qpVec< T, 3 > Forward() const;
	qpVec< T, 3 > Right() const;
	qpVec< T, 3 > Up() const;

	qpMat< T, 3 > ToMat3() const;
	qpMat< T, 4 > ToMat4() const;

	qpQuaternion operator-() const;

	qpVec< T, 3 > Axis() const;

	T * Data() { return &x; }
	const T * Data() const { return &x; }
private:
};

template< typename T >
qpQuaternion< T >::qpQuaternion( const T & _x, const T & _y, const T & _z, const T & _w )
	: x( _x ), y( _y ), z( _z ), w( _w ) {
}

template< typename T >
qpQuaternion<T>::qpQuaternion( const T & xDeg, const T & yDeg, const T & zDeg ) {
	qpVec< T, 3 > halfAngleRad( qpMath::Deg2Rad( xDeg * static_cast< T >( 0.5 ) ),
								 qpMath::Deg2Rad( yDeg * static_cast< T >( 0.5 ) ),
								 qpMath::Deg2Rad( zDeg * static_cast< T >( 0.5 ) ) );

	const qpQuaternion< T > pitch { qpMath::Sin( halfAngleRad.x ), static_cast< T >( 0 ), static_cast< T >( 0 ), qpMath::Cos( halfAngleRad.x ) };
	const qpQuaternion< T > yaw { static_cast< T >( 0 ), qpMath::Sin( halfAngleRad.y ), static_cast< T >( 0 ), qpMath::Cos( halfAngleRad.y ) };
	const qpQuaternion< T > roll { static_cast< T >( 0 ), static_cast< T >( 0 ), qpMath::Sin( halfAngleRad.z ), qpMath::Cos( halfAngleRad.z ) };

	qpQuaternion< T > result = roll * yaw * pitch;
	result.Normalize();
	x = result.x;
	y = result.y;
	z = result.z;
	w = result.w;
}

template< typename T >
qpQuaternion<T>::qpQuaternion( const qpVec<T, 3> & axis, const T & angleDeg ) {
	T halfAngleRad = qpMath::Deg2Rad( angleDeg * static_cast< T >( 0.5 ) );
	T sinHalfAngle = qpMath::Sin( halfAngleRad );
	x = axis.x * sinHalfAngle;
	y = axis.y * sinHalfAngle;
	z = axis.z * sinHalfAngle;
	w = qpMath::Cos( halfAngleRad );
}

//From: https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
template< typename T >
qpQuaternion<T>::qpQuaternion( const qpMat<T, 3> & mat ) {
	const T m00 = mat( 1, 1 );
	const T m01 = mat( 2, 1 );
	const T m02 = mat( 3, 1 );

	const T m10 = mat( 1, 2 );
	const T m11 = mat( 2, 2 );
	const T m12 = mat( 3, 2 );

	const T m20 = mat( 1, 3 );
	const T m21 = mat( 2, 3 );
	const T m22 = mat( 3, 3 );

	qpQuaternion< T > quatFromMatrix;

	T t = static_cast< T >( 0 );
	if ( m22 < static_cast< T >( 0 ) ) {
		if ( m00 > m11 ) {
			t = static_cast< T >( 1 ) + m00 - m11 - m22;

			quatFromMatrix.x = t;
			quatFromMatrix.y = m01 + m10;
			quatFromMatrix.z = m20 + m02;
			quatFromMatrix.w = m12 - m21;
		} else {
			t = static_cast< T >( 1 ) - m00 + m11 - m22;
			quatFromMatrix.x = m01 + m10;
			quatFromMatrix.y = t;
			quatFromMatrix.z = m12 + m21;
			quatFromMatrix.w = m20 - m02;
		}
	} else {
		if ( m00 < -m11 ) {
			t = static_cast< T >( 1 ) - m00 - m11 + m22;
			quatFromMatrix.x = m20 + m02;
			quatFromMatrix.y = m12 + m21;
			quatFromMatrix.z = t;
			quatFromMatrix.w = m01 - m10;
		} else {
			t = static_cast< T >( 1 ) + m00 + m11 + m22;
			quatFromMatrix.x = m12 - m21;
			quatFromMatrix.y = m20 - m02;
			quatFromMatrix.z = m01 - m10;
			quatFromMatrix.w = t;
		}
	}

	quatFromMatrix *= static_cast< T >( 0.5 ) / qpMath::Sqrt( t );
	quatFromMatrix.w *= static_cast< T >( -1 );
	quatFromMatrix.Normalize();

	x = quatFromMatrix.x;
	y = quatFromMatrix.y;
	z = quatFromMatrix.z;
	w = quatFromMatrix.w;
}

template< typename T >
qpQuaternion<T>::qpQuaternion( const qpMat<T, 4> & mat )
	: qpQuaternion( static_cast< qpVec< T, 3 > >( mat.Right() ), static_cast< qpVec< T, 3 > >( mat.Up() ), static_cast< qpVec< T, 3 > >( mat.Forward() ) ){
}

template< typename T >
T qpQuaternion<T>::Length() const {
	return qpMath::Sqrt( LengthSqr() );
}

template< typename T >
T qpQuaternion<T>::LengthSqr() const {
	return qpDot( *this, *this );
}

template< typename T >
qpQuaternion<T> qpQuaternion<T>::Normalized() const {
	qpQuaternion< T > result;
	result.Normalize();
	return result;
}

template< typename T >
void qpQuaternion<T>::Normalize() {
	T magnitude = Length();

	if ( magnitude != static_cast< T >( 0 ) ) {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;
	}
}

template< typename T >
qpVec<T, 3> qpQuaternion<T>::ToEulers() const {
	qpVec< T, 3 > angles( 0.0f );

	// x-axis rotation
	T sinXCosY = static_cast< T >( 2 ) * ( w * x + y * z );
	T cosXSinY = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( x * x + y * y );
	angles.x = qpMath::Atan2( sinXCosY, cosXSinY );

	// y-axis rotation
	T sinY = static_cast< T >( 2 ) * ( w * y - z * x );
	if ( qpMath::Abs( sinY ) >= static_cast< T >( 1 ) ) {
		angles.y = qpMath::CopySign( qpMath::Pi / 2, sinY ); // use 90 degrees if out of range
	} else {
		angles.y = qpMath::Asin( sinY );
	}

	// z-axis rotation
	T sinZCosY = static_cast< T >( 2 ) * ( w * z + x * y );
	T cosZCosY = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( y * y + z * z );
	angles.z = qpMath::Atan2( sinZCosY, cosZCosY );

	return angles;
}

template< typename T >
qpVec<T, 3> qpQuaternion<T>::Forward() const {
	qpVec< T, 3 > forward;
	forward.x = static_cast< T >( 2 ) * ( x * z + w * y );
	forward.y = static_cast< T >( 2 ) * ( y * z - w * x );
	forward.z = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( x * x + y * y );
	return forward;
}

template< typename T >
qpVec<T, 3> qpQuaternion<T>::Right() const {
	qpVec< T, 3 > right;
	right.x = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( y * y + z * z );
	right.y = static_cast< T >( 2 ) * ( x * y + w * z );
	right.z = static_cast< T >( 2 ) * ( x * z - w * y );
	return right;
}

template< typename T >
qpVec<T, 3> qpQuaternion<T>::Up() const {
	qpVec< T, 3 > up;
	up.x = static_cast< T >( 2 ) * ( x * y - w * z );
	up.y = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( x * x + z * z );
	up.z = static_cast< T >( 2 ) * ( y * z + w * x );
	return up;
}

template< typename T >
qpMat<T, 3> qpQuaternion<T>::ToMat3() const {
	qpMat< T, 3 > rotationMatrix;

	rotationMatrix( 1, 1 ) = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( y * y + z * z );
	rotationMatrix( 1, 2 ) = static_cast< T >( 2 ) * ( x * y + w * z );
	rotationMatrix( 1, 3 ) = static_cast< T >( 2 ) * ( x * z - w * y );

	rotationMatrix( 2, 1 ) = static_cast< T >( 2 ) * ( x * y - w * z );
	rotationMatrix( 2, 2 ) = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( x * x + z * z );
	rotationMatrix( 2, 3 ) = static_cast< T >( 2 ) * ( y * z + w * x );

	rotationMatrix( 3, 1 ) = static_cast< T >( 2 ) * ( x * z + w * y );
	rotationMatrix( 3, 2 ) = static_cast< T >( 2 ) * ( y * z - w * x );
	rotationMatrix( 3, 3 ) = static_cast< T >( 1 ) - static_cast< T >( 2 ) * ( x * x + y * y );

	return rotationMatrix;
}

template< typename T >
qpMat<T, 4> qpQuaternion<T>::ToMat4() const {
	const qpMat< T, 3 > rotationMatrix = ToMat3();
	return qpMat< T, 4 > {
		qpVec< T, 4 >( rotationMatrix.Right(), static_cast< T >( 0 ) ),
		qpVec< T, 4 >( rotationMatrix.Up(), static_cast< T >( 0 ) ),
		qpVec< T, 4 >( rotationMatrix.Forward(), static_cast< T >( 0 ) ),
		qpVec< T, 4 >( static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 0 ), static_cast< T >( 1 ) ),
	};
}

template< typename T >
qpQuaternion<T> qpQuaternion<T>::operator-() const {
	return qpQuaternion< T >( -x, -y, -z, -w );
}

template< typename T >
qpVec<T, 3> qpQuaternion<T>::Axis() const {
	return qpVec< T, 3 >( x, y, z );
}

template < typename T >
static qpQuaternion< T > & operator*=( qpQuaternion< T > & lhs, const qpQuaternion< T > & rhs ) {
	T x = lhs.x;
	T y = lhs.y;
	T z = lhs.z;
	T w = lhs.w;

	lhs.x = ( w * rhs.x ) + ( x * rhs.w ) + ( y * rhs.z ) - ( z * rhs.y );
	lhs.y = ( w * rhs.y ) - ( x * rhs.z ) + ( y * rhs.w ) + ( z * rhs.x );
	lhs.z = ( w * rhs.z ) + ( x * rhs.y ) - ( y * rhs.x ) + ( z * rhs.w );
	lhs.w = ( w * rhs.w ) - ( x * rhs.x ) - ( y * rhs.y ) - ( z * rhs.z );

	return lhs;
}

template < typename T >
static qpQuaternion< T > & operator*=( qpQuaternion< T > & lhs, const T & rhs ) {
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
	return lhs;
}

template < typename T >
static qpQuaternion< T > operator*( const qpQuaternion< T > & lhs, const qpQuaternion< T > & rhs ) {
	qpQuaternion< T > result = lhs;
	result *= rhs;
	return result;
}

template < typename T >
static qpQuaternion< T > operator*( const qpQuaternion< T > & lhs, const T & rhs ) {
	qpQuaternion< T > result = lhs;
	result *= rhs;
	return result;
}

template < typename T >
static qpQuaternion< T > operator*( const T & lhs, const qpQuaternion< T > & rhs ) {
	qpQuaternion< T > result = rhs;
	result *= lhs;
	return result;
}

template < typename T >
static qpQuaternion< T > & operator/=( qpQuaternion< T > & lhs, const T & rhs ) {
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	lhs.w /= rhs.w;
	return lhs;
}

template < typename T >
static qpQuaternion< T > operator/( const qpQuaternion< T > & lhs, const T & rhs ) {
	qpQuaternion< T > result = lhs;
	result /= rhs;
	return result;
}

template < typename T >
static qpQuaternion< T > operator/( const T & lhs, const qpQuaternion< T > & rhs ) {
	qpQuaternion< T > result = rhs;
	result /= lhs;
	return result;
}

template < typename T >
static qpQuaternion< T > & operator+=( qpQuaternion< T > & lhs, const qpQuaternion< T > & rhs ) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

template < typename T >
static qpQuaternion< T > operator+( const qpQuaternion< T > & lhs, const qpQuaternion< T > & rhs ) {
	qpQuaternion< T > result = lhs;
	result += rhs;
	return result;
}

template < typename T >
static qpQuaternion< T > qpConjugate( const qpQuaternion< T > & quat ) {
	return qpQuaternion< T >( -quat.x, -quat.y, -quat.z, quat.w );
}

template < typename T >
static qpQuaternion< T > qpLookRotation( const qpVec< T, 3 > & forward, const qpVec< T, 3 > & up ) {
	qpVec< T, 3 > lookRight = qpCross( up, forward );
	qpVec< T, 3 > lookUp = qpCross( forward, lookRight );
	return qpQuaternion< T >( qpMat< T, 3 >( lookRight, lookUp, forward ) );
}

template < typename T >
static qpQuaternion< T > qpLookRotation( const qpVec< T, 3 > & forward ) {
	return qpLookRotation( forward, g_vec3Up );
}

template < typename T >
static qpQuaternion< T > qpFromMat3( const qpMat< T, 3 > & mat ) {
	return qpQuaternion< T >( mat );
}

template < typename T >
static qpQuaternion< T > qpFromMat4( const qpMat< T, 4 > & mat ) {
	return qpQuaternion< T >( mat );
}

template < typename T >
static qpQuaternion< T > qpFromEulers( const T & xDeg, const T & yDeg, const T & zDeg ) {
	return qpQuaternion< T >( xDeg, yDeg, zDeg );
}

template < typename T >
static qpQuaternion< T > qpFromEulers( const qpVec< T, 3 > & eulersDeg ) {
	return qpFromEulers( eulersDeg.x, eulersDeg.y, eulersDeg.z );
}

template < typename T >
static qpQuaternion< T > qpFromAxisAngle( const qpVec< T, 3 > & axis, const T & angleDeg ) {
	return qpQuaternion< T >( axis, angleDeg );
}

template < typename T >
static qpQuaternion< T > qpLerp( const qpQuaternion< T > & from, const qpQuaternion< T > & to, const T & time ) {
	return qpMath::Lerp( from, to, time );
}

template < typename T >
static qpQuaternion< T > qpSlerp( const qpQuaternion< T > & from, const qpQuaternion< T > & to, const T & time ) {
	const qpQuaternion< T > a = from;
	qpQuaternion< T > b = to;

	T dot = qpDot( a, b );

	if ( dot < static_cast< T >( 0 ) ) {
		b = -b;
		dot *= static_cast< T >( -1 );
	}

	T dotABSqr = qpMath::Min( dot * dot, static_cast< T >( 1 ) );
	T sinTheta = qpMath::Sqrt( static_cast< T >( 1 ) - dotABSqr );
	if ( sinTheta < static_cast< T >( 0.0001 ) ) {
		return a;
	}

	return ( ( sinTheta * ( static_cast< T >( 1 ) - time ) ) / sinTheta ) * a + ( ( sinTheta * time ) / sinTheta ) * b;
}

template < typename T >
static T qpDot( const qpQuaternion< T > & lhs, const qpQuaternion< T > & rhs ) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

template < typename T >
qpQuaternion< T > qpDeltaRotationLocal( const qpQuaternion< T > & from, const qpQuaternion< T > & to ) {
	return  qpConjugate( from ) * to;
}

template < typename T >
qpQuaternion< T > qpDeltaRotationWorld( const qpQuaternion< T > & from, const qpQuaternion< T > & to ) {
	return  to * qpConjugate( from );
}

using qpQuat = qpQuaternion< float >;
using qpQuatd = qpQuaternion< double >;


static const qpQuat g_quatIdentity { 0.0f, 0.0f, 0.0f, 1.0f };