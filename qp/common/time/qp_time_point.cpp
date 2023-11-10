#include "qp_time_point.h"

qpTimePoint::qpTimePoint( int64 ticks, int64 ticksPerSecond ) {
	m_ticks = ticks;
	m_ticksPerSecond = ticksPerSecond;
}

bool qpTimePoint::operator==( const qpTimePoint & rhs ) const {
	return ( m_ticks == rhs.m_ticks ) && ( m_ticksPerSecond == rhs.m_ticksPerSecond );
}

qpTimePoint & qpTimePoint::operator-=( const qpTimePoint & rhs ) {
	m_ticks -= rhs.m_ticks;
	return *this;
}

qpTimePoint operator-( const qpTimePoint & lhs, const qpTimePoint & rhs ) {
	qpTimePoint result( lhs );
	result -= rhs;
	return result;
}
