#pragma once
#include "qp/common/core/qp_types.h"
#include <compare>

class qpTimePoint {
	friend class qpClock;
public:
	int64 AsNanoseconds() const { return ( m_ticks * 1000ll * 1000ll * 1000ll ) / m_ticksPerSecond; }
	int64 AsMicroseconds() const { return ( m_ticks * 1000ll * 1000ll ) / m_ticksPerSecond; }
	int64 AsMilliseconds() const { return ( m_ticks * 1000ll ) / m_ticksPerSecond; }
	float AsSeconds() const { return static_cast< float >( m_ticks ) / static_cast< float >( m_ticksPerSecond ); }
	int64 AsFPS() const { int64 microseconds = AsMicroseconds(); return ( microseconds == 0 ) ? 0 : ( ( 1000ll * 1000ll ) / microseconds ); }

	auto operator<=>( const qpTimePoint & rhs ) const = default;
	bool operator==( const qpTimePoint & rhs ) const { return ( m_ticks == rhs.m_ticks ) && ( m_ticksPerSecond == rhs.m_ticksPerSecond ); }

	qpTimePoint & operator-=( const qpTimePoint & rhs );

	friend qpTimePoint operator-( const qpTimePoint & lhs, const qpTimePoint & rhs );
private:
	qpTimePoint( int64 ticks, int64 ticksPerSecond );

	int64 m_ticks = 0;
	int64 m_ticksPerSecond = 0;
};
