#pragma once
#include "qp_time_types.h"
#include "qp/common/core/qp_types.h"
#include <compare>

namespace qp {
	class TimePoint {
		friend class Clock;
	public:
		TimePoint( const timeTick_t ticks, timeTick_t ticksPerSecond );
		TimePoint( const nanoseconds_t nanoseconds );
		TimePoint( const microseconds_t microseconds );
		TimePoint( const milliseconds_t milliseconds );
		TimePoint( const seconds_t seconds );

		nanoseconds_t AsNanoseconds() const { return nanoseconds_t( m_ticks, m_ticksPerSecond ); }
		microseconds_t AsMicroseconds() const { return microseconds_t( m_ticks, m_ticksPerSecond ); }
		milliseconds_t AsMilliseconds() const { return milliseconds_t( m_ticks, m_ticksPerSecond ); }
		seconds_t AsSeconds() const { return seconds_t( m_ticks, m_ticksPerSecond ); }
		int64 AsFPS() const { int64 microseconds = AsMicroseconds().Get(); return ( microseconds == 0 ) ? 0 : ( ( 1000ll * 1000ll ) / microseconds ); }

		auto operator<=>( const TimePoint & rhs ) const = default;
		bool operator==( const TimePoint & rhs ) const { return ( m_ticks == rhs.m_ticks ) && ( m_ticksPerSecond == rhs.m_ticksPerSecond ); }

		TimePoint & operator-=( const TimePoint & rhs );

		timeTick_t GetTicksPerSecond() const { return m_ticksPerSecond; }

		friend TimePoint operator-( const TimePoint & lhs, const TimePoint & rhs );
	private:
		timeTick_t m_ticks = 0;
		timeTick_t m_ticksPerSecond = 0;
	};

	const TimePoint g_timePointInfinity( INT64_MAX, INT64_MAX );
	const TimePoint g_timePointInvalid( 0, -1 );
}