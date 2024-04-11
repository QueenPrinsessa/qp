#pragma once
#include "qp_mat.h"
#include "qp_math.h"
#include "qp_vec.h"

template < typename _type_ >
class qpQuaternion {
public:
	_type_ x = static_cast< _type_ >( 0 );
	_type_ y = static_cast< _type_ >( 0 );
	_type_ z = static_cast< _type_ >( 0 );
	_type_ w = static_cast< _type_ >( 1 );

	qpQuaternion() = default;
	qpQuaternion( const _type_ & _x, const _type_ & _y, const _type_ & _z, const _type_ & _w );
	qpQuaternion( const _type_ & xDeg, const _type_ & yDeg, const _type_ & zDeg );
	qpQuaternion( const qpVec< _type_, 3 > & axis, const _type_ & angleDeg );
	explicit qpQuaternion( const qpMat< _type_, 3 > & mat );
	explicit qpQuaternion( const qpMat< _type_, 4 > & mat );

	_type_ Length() const;
	_type_ LengthSqr() const;

	qpQuaternion Normalized() const;
	void Normalize();

	qpVec< _type_, 3 > ToEulers() const;

	qpVec< _type_, 3 > Forward() const;
	qpVec< _type_, 3 > Right() const;
	qpVec< _type_, 3 > Up() const;

	qpMat< _type_, 3 > ToMat3() const;
	qpMat< _type_, 4 > ToMat4() const;

	qpQuaternion operator-() const;

	qpVec< _type_, 3 > Axis() const;

	_type_ * Data() { return &x; }
	const _type_ * Data() const { return &x; }
private:
};

template< typename _type_ >
qpQuaternion< _type_ >::qpQuaternion( const _type_ & _x, const _type_ & _y, const _type_ & _z, const _type_ & _w )
	: x( _x ), y( _y ), z( _z ), w( _w ) {
}

template< typename _type_ >
qpQuaternion< _type_ >::qpQuaternion( const _type_ & xDeg, const _type_ & yDeg, const _type_ & zDeg ) {
	qpVec< _type_, 3 > halfAngleRad( qpMath::Deg2Rad( xDeg * static_cast< _type_ >( 0.5 ) ),
								 qpMath::Deg2Rad( yDeg * static_cast< _type_ >( 0.5 ) ),
								 qpMath::Deg2Rad( zDeg * static_cast< _type_ >( 0.5 ) ) );

	const qpQuaternion< _type_ > pitch { qpMath::Sin( halfAngleRad.x ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), qpMath::Cos( halfAngleRad.x ) };
	const qpQuaternion< _type_ > yaw { static_cast< _type_ >( 0 ), qpMath::Sin( halfAngleRad.y ), static_cast< _type_ >( 0 ), qpMath::Cos( halfAngleRad.y ) };
	const qpQuaternion< _type_ > roll { static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), qpMath::Sin( halfAngleRad.z ), qpMath::Cos( halfAngleRad.z ) };

	qpQuaternion< _type_ > result = roll * yaw * pitch;
	result.Normalize();
	x = result.x;
	y = result.y;
	z = result.z;
	w = result.w;
}

template< typename _type_ >
qpQuaternion<  _type_  >::qpQuaternion( const qpVec<_type_, 3> & axis, const _type_ & angleDeg ) {
	_type_ halfAngleRad = qpMath::Deg2Rad( angleDeg * static_cast< _type_ >( 0.5 ) );
	_type_ sinHalfAngle = qpMath::Sin( halfAngleRad );
	x = axis.x * sinHalfAngle;
	y = axis.y * sinHalfAngle;
	z = axis.z * sinHalfAngle;
	w = qpMath::Cos( halfAngleRad );
}

//From: https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
template< typename _type_ >
qpQuaternion<  _type_  >::qpQuaternion( const qpMat<_type_, 3> & mat ) {
	const _type_ m00 = mat( 1, 1 );
	const _type_ m01 = mat( 2, 1 );
	const _type_ m02 = mat( 3, 1 );

	const _type_ m10 = mat( 1, 2 );
	const _type_ m11 = mat( 2, 2 );
	const _type_ m12 = mat( 3, 2 );

	const _type_ m20 = mat( 1, 3 );
	const _type_ m21 = mat( 2, 3 );
	const _type_ m22 = mat( 3, 3 );

	qpQuaternion< _type_ > quatFromMatrix;

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

	quatFromMatrix *= static_cast< _type_ >( 0.5 ) / qpMath::Sqrt( t );
	quatFromMatrix.w *= static_cast< _type_ >( -1 );
	quatFromMatrix.Normalize();

	x = quatFromMatrix.x;
	y = quatFromMatrix.y;
	z = quatFromMatrix.z;
	w = quatFromMatrix.w;
}

template< typename _type_ >
qpQuaternion<  _type_  >::qpQuaternion( const qpMat<_type_, 4> & mat )
	: qpQuaternion( static_cast< qpVec< _type_, 3 > >( mat.Right() ), static_cast< qpVec< _type_, 3 > >( mat.Up() ), static_cast< qpVec< _type_, 3 > >( mat.Forward() ) ){
}

template< typename _type_ >
_type_ qpQuaternion<  _type_  >::Length() const {
	return qpMath::Sqrt( LengthSqr() );
}

template< typename _type_ >
_type_ qpQuaternion<  _type_  >::LengthSqr() const {
	return qpDot( *this, *this );
}

template< typename _type_ >
qpQuaternion<  _type_  > qpQuaternion<  _type_  >::Normalized() const {
	qpQuaternion< _type_ > result;
	result.Normalize();
	return result;
}

template< typename _type_ >
void qpQuaternion<  _type_  >::Normalize() {
	_type_ magnitude = Length();

	if ( magnitude != static_cast< _type_ >( 0 ) ) {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;
	}
}

template< typename _type_ >
qpVec<_type_, 3> qpQuaternion<  _type_  >::ToEulers() const {
	qpVec< _type_, 3 > angles( 0.0f );

	// x-axis rotation
	_type_ sinXCosY = static_cast< _type_ >( 2 ) * ( w * x + y * z );
	_type_ cosXSinY = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + y * y );
	angles.x = qpMath::Atan2( sinXCosY, cosXSinY );

	// y-axis rotation
	_type_ sinY = static_cast< _type_ >( 2 ) * ( w * y - z * x );
	if ( qpMath::Abs( sinY ) >= static_cast< _type_ >( 1 ) ) {
		angles.y = qpMath::CopySign( qpMath::Pi / 2, sinY ); // use 90 degrees if out of range
	} else {
		angles.y = qpMath::Asin( sinY );
	}

	// z-axis rotation
	_type_ sinZCosY = static_cast< _type_ >( 2 ) * ( w * z + x * y );
	_type_ cosZCosY = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( y * y + z * z );
	angles.z = qpMath::Atan2( sinZCosY, cosZCosY );

	return angles;
}

template< typename _type_ >
qpVec<_type_, 3> qpQuaternion<  _type_  >::Forward() const {
	qpVec< _type_, 3 > forward;
	forward.x = static_cast< _type_ >( 2 ) * ( x * z + w * y );
	forward.y = static_cast< _type_ >( 2 ) * ( y * z - w * x );
	forward.z = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + y * y );
	return forward;
}

template< typename _type_ >
qpVec<_type_, 3> qpQuaternion<  _type_  >::Right() const {
	qpVec< _type_, 3 > right;
	right.x = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( y * y + z * z );
	right.y = static_cast< _type_ >( 2 ) * ( x * y + w * z );
	right.z = static_cast< _type_ >( 2 ) * ( x * z - w * y );
	return right;
}

template< typename _type_ >
qpVec<_type_, 3> qpQuaternion<  _type_  >::Up() const {
	qpVec< _type_, 3 > up;
	up.x = static_cast< _type_ >( 2 ) * ( x * y - w * z );
	up.y = static_cast< _type_ >( 1 ) - static_cast< _type_ >( 2 ) * ( x * x + z * z );
	up.z = static_cast< _type_ >( 2 ) * ( y * z + w * x );
	return up;
}

template< typename _type_ >
qpMat<_type_, 3> qpQuaternion<  _type_  >::ToMat3() const {
	qpMat< _type_, 3 > rotationMatrix;

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
qpMat<_type_, 4> qpQuaternion<  _type_  >::ToMat4() const {
	const qpMat< _type_, 3 > rotationMatrix = ToMat3();
	return qpMat< _type_, 4 > {
		qpVec< _type_, 4 >( rotationMatrix.Right(), static_cast< _type_ >( 0 ) ),
		qpVec< _type_, 4 >( rotationMatrix.Up(), static_cast< _type_ >( 0 ) ),
		qpVec< _type_, 4 >( rotationMatrix.Forward(), static_cast< _type_ >( 0 ) ),
		qpVec< _type_, 4 >( static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 0 ), static_cast< _type_ >( 1 ) ),
	};
}

template< typename _type_ >
qpQuaternion<  _type_  > qpQuaternion<  _type_  >::operator-() const {
	return qpQuaternion< _type_ >( -x, -y, -z, -w );
}

template< typename _type_ >
qpVec<_type_, 3> qpQuaternion<  _type_  >::Axis() const {
	return qpVec< _type_, 3 >( x, y, z );
}

template < typename _type_ >
static qpQuaternion< _type_ > & operator*=( qpQuaternion< _type_ > & lhs, const qpQuaternion< _type_ > & rhs ) {
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
static qpQuaternion< _type_ > & operator*=( qpQuaternion< _type_ > & lhs, const _type_ & rhs ) {
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
	return lhs;
}

template < typename _type_ >
static qpQuaternion< _type_ > operator*( const qpQuaternion< _type_ > & lhs, const qpQuaternion< _type_ > & rhs ) {
	qpQuaternion< _type_ > result = lhs;
	result *= rhs;
	return result;
}

template < typename _type_ >
static qpQuaternion< _type_ > operator*( const qpQuaternion< _type_ > & lhs, const _type_ & rhs ) {
	qpQuaternion< _type_ > result = lhs;
	result *= rhs;
	return result;
}

template < typename _type_ >
static qpQuaternion< _type_ > operator*( const _type_ & lhs, const qpQuaternion< _type_ > & rhs ) {
	qpQuaternion< _type_ > result = rhs;
	result *= lhs;
	return result;
}

template < typename _type_ >
static qpQuaternion< _type_ > & operator/=( qpQuaternion< _type_ > & lhs, const _type_ & rhs ) {
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	lhs.w /= rhs.w;
	return lhs;
}

template < typename _type_ >
static qpQuaternion< _type_ > operator/( const qpQuaternion< _type_ > & lhs, const _type_ & rhs ) {
	qpQuaternion< _type_ > result = lhs;
	result /= rhs;
	return result;
}

template < typename _type_ >
static qpQuaternion< _type_ > operator/( const _type_ & lhs, const qpQuaternion< _type_ > & rhs ) {
	qpQuaternion< _type_ > result = rhs;
	result /= lhs;
	return result;
}

template < typename _type_ >
static qpQuaternion< _type_ > & operator+=( qpQuaternion< _type_ > & lhs, const qpQuaternion< _type_ > & rhs ) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

template < typename _type_ >
static qpQuaternion< _type_ > operator+( const qpQuaternion< _type_ > & lhs, const qpQuaternion< _type_ > & rhs ) {
	qpQuaternion< _type_ > result = lhs;
	result += rhs;
	return result;
}

template < typename _type_ >
static qpQuaternion< _type_ > qpConjugate( const qpQuaternion< _type_ > & quat ) {
	return qpQuaternion< _type_ >( -quat.x, -quat.y, -quat.z, quat.w );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpLookRotation( const qpVec< _type_, 3 > & forward, const qpVec< _type_, 3 > & up ) {
	qpVec< _type_, 3 > lookRight = qpCross( up, forward );
	qpVec< _type_, 3 > lookUp = qpCross( forward, lookRight );
	return qpQuaternion< _type_ >( qpMat< _type_, 3 >( lookRight, lookUp, forward ) );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpLookRotation( const qpVec< _type_, 3 > & forward ) {
	return qpLookRotation( forward, g_vec3Up );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpFromMat3( const qpMat< _type_, 3 > & mat ) {
	return qpQuaternion< _type_ >( mat );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpFromMat4( const qpMat< _type_, 4 > & mat ) {
	return qpQuaternion< _type_ >( mat );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpFromEulers( const _type_ & xDeg, const _type_ & yDeg, const _type_ & zDeg ) {
	return qpQuaternion< _type_ >( xDeg, yDeg, zDeg );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpFromEulers( const qpVec< _type_, 3 > & eulersDeg ) {
	return qpFromEulers( eulersDeg.x, eulersDeg.y, eulersDeg.z );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpFromAxisAngle( const qpVec< _type_, 3 > & axis, const _type_ & angleDeg ) {
	return qpQuaternion< _type_ >( axis, angleDeg );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpLerp( const qpQuaternion< _type_ > & from, const qpQuaternion< _type_ > & to, const _type_ & time ) {
	return qpMath::Lerp( from, to, time );
}

template < typename _type_ >
static qpQuaternion< _type_ > qpSlerp( const qpQuaternion< _type_ > & from, const qpQuaternion< _type_ > & to, const _type_ & time ) {
	const qpQuaternion< _type_ > a = from;
	qpQuaternion< _type_ > b = to;

	_type_ dot = qpDot( a, b );

	if ( dot < static_cast< _type_ >( 0 ) ) {
		b = -b;
		dot *= static_cast< _type_ >( -1 );
	}

	_type_ dotABSqr = qpMath::Min( dot * dot, static_cast< _type_ >( 1 ) );
	_type_ sinTheta = qpMath::Sqrt( static_cast< _type_ >( 1 ) - dotABSqr );
	if ( sinTheta < static_cast< _type_ >( 0.0001 ) ) {
		return a;
	}

	return ( ( sinTheta * ( static_cast< _type_ >( 1 ) - time ) ) / sinTheta ) * a + ( ( sinTheta * time ) / sinTheta ) * b;
}

template < typename _type_ >
static _type_ qpDot( const qpQuaternion< _type_ > & lhs, const qpQuaternion< _type_ > & rhs ) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

template < typename _type_ >
qpQuaternion< _type_ > qpDeltaRotationLocal( const qpQuaternion< _type_ > & from, const qpQuaternion< _type_ > & to ) {
	return  qpConjugate( from ) * to;
}

template < typename _type_ >
qpQuaternion< _type_ > qpDeltaRotationWorld( const qpQuaternion< _type_ > & from, const qpQuaternion< _type_ > & to ) {
	return  to * qpConjugate( from );
}

using qpQuat = qpQuaternion< float >;
using qpQuatd = qpQuaternion< double >;


static const qpQuat g_quatIdentity { 0.0f, 0.0f, 0.0f, 1.0f };