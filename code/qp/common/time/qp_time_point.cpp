#include "engine.pch.h"
#include "qp_time_point.h"
#include "qp_clock.h"
#include "common/math/qp_math.h"

qpTimePoint::qpTimePoint( int64 ticks, int64 ticksPerSecond ) {
	m_ticks = ticks;
	m_ticksPerSecond = ticksPerSecond;
}

qpTimePoint::qpTimePoint( const nanoseconds_t nanoseconds )
	: m_ticks( nanoseconds.GetTicks() ), m_ticksPerSecond( g_ticksPerSecond ) {}


qpTimePoint::qpTimePoint( const microseconds_t microseconds )
	: m_ticks( microseconds.GetTicks() ), m_ticksPerSecond( g_ticksPerSecond ) {}


qpTimePoint::qpTimePoint( const milliseconds_t milliseconds )
	: m_ticks( milliseconds.GetTicks() ), m_ticksPerSecond( g_ticksPerSecond ) {}

qpTimePoint::qpTimePoint ( const seconds_t seconds ) 
	: m_ticks( seconds.GetTicks() ), m_ticksPerSecond( g_ticksPerSecond ) {}

qpTimePoint & qpTimePoint::operator-=( const qpTimePoint & rhs ) {
	m_ticks -= llround( static_cast< double >( rhs.m_ticks ) * static_cast< double >( m_ticksPerSecond ) / static_cast< double >( rhs.m_ticksPerSecond ) );
	return *this;
}

qpTimePoint operator-( const qpTimePoint & lhs, const qpTimePoint & rhs ) {
	qpTimePoint result( lhs );
	result -= rhs;
	return result;
}
