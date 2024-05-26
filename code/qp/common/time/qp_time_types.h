#pragma once
#include "common/utilities/qp_algorithms.h"
#include <compare>

using timeTick_t = int64;
const timeTick_t g_ticksPerSecond = 1000000000ll; // nanosecond precision

template < timeTick_t _ticksPerUnit_, timeTick_t _ticksPerSecond_ = g_ticksPerSecond >
class qpTimeType {
public:
	explicit qpTimeType( const timeTick_t value ) 
		: m_value( value ), m_ticks( value * _ticksPerUnit_ ) {}
	explicit qpTimeType( const timeTick_t ticks, const timeTick_t ticksPerSecond ) {
		SetFromTicks( ticks, ticksPerSecond );
	}
	template < timeTick_t _otherTicksPerUnit_ >
	explicit qpTimeType( const qpTimeType< _otherTicksPerUnit_ > & other ) 
		: m_value( other.m_ticks / _ticksPerUnit_ ), m_ticks( other.m_ticks ) {}
	template < timeTick_t _otherTicksPerUnit_ >
	qpTimeType & operator=( const qpTimeType< _otherTicksPerUnit_ > & other )
		: m_value( other.m_ticks / _ticksPerUnit_ ), m_ticks( other.m_ticks ) {
		return *this;
	}
	void SetFromTicks( const timeTick_t ticks ) { 
		m_ticks = ticks;
		m_value = ticks / _ticksPerUnit_;
	}
	void SetFromTicks( const timeTick_t ticks, const timeTick_t ticksPerSecond ) {
		timeTick_t scaledTicks = ticks;
		if ( ticksPerSecond != _ticksPerSecond_ ) {
			scaledTicks = llround( static_cast< double >( ticks ) * static_cast< double >( _ticksPerUnit_ ) / static_cast< double >( ticksPerSecond ) );
		}
		SetFromTicks( scaledTicks );
	}
	template < timeTick_t _otherTicksPerUnit_ >
	void SetFromTimeType( const qpTimeType< _otherTicksPerUnit_ > & timeType ) { SetFromTicks( timeType.m_ticks ); }

	timeTick_t Get() const { return m_value; }
	timeTick_t Get( const timeTick_t ticksPerUnit ) const { return m_value * ticksPerUnit; }
	int GetI32() const { return qpVerifyStaticCast< int >( m_value ); }
	float GetFloat() const { return static_cast< float >( m_ticks ) / static_cast< float >( _ticksPerUnit_ ); }
	double GetDouble() const { return static_cast< double >( m_ticks ) / static_cast< double >( _ticksPerUnit_ ); }
	operator timeTick_t () const { return m_value; }
	timeTick_t GetTicks() const { return m_ticks; }

	static constexpr timeTick_t TicksPerUnit() { return _ticksPerUnit_; }
	
	auto operator<=>( const qpTimeType & rhs ) const = default;
	bool operator==( const qpTimeType & rhs ) const { return m_value == rhs.m_value; }

	template < timeTick_t _otherTicksPerUnit_ >
	auto operator<=>( const qpTimeType< _otherTicksPerUnit_ > & rhs ) {
		return m_ticks <=> rhs.m_ticks;
	}
	template < timeTick_t _otherTicksPerUnit_ >
	auto operator==( const qpTimeType< _otherTicksPerUnit_ > & rhs ) {
		return m_ticks == rhs.m_ticks;
	}

	template < timeTick_t _otherTicksPerUnit_ >
	qpTimeType & operator-=( const qpTimeType< _otherTicksPerUnit_ > & rhs ) {
		SetFromTicks( m_ticks - rhs.m_ticks );
		return *this;
	}
	template < timeTick_t _otherTicksPerUnit_ >
	qpTimeType operator-( const qpTimeType< _otherTicksPerUnit_ > & rhs ) {
		qpTimeType time;
		time -= rhs;
		return time;
	}
	template < timeTick_t _otherTicksPerUnit_ >
	qpTimeType & operator+=( const qpTimeType< _otherTicksPerUnit_ > & rhs ) {
		SetFromTicks( m_ticks + rhs.m_ticks );
		return *this;
	}
	template < timeTick_t _otherTicksPerUnit_ >
	qpTimeType operator+( const qpTimeType< _otherTicksPerUnit_ > & rhs ) {
		qpTimeType time;
		time += rhs;
		return time;
	}
private:
	timeTick_t m_value = 0ll;
	timeTick_t m_ticks = 0ll;
};
#define DECLARE_TIME_TYPE( timeType, ticks ) using timeType = qpTimeType< ticks >
DECLARE_TIME_TYPE( nanoseconds_t, g_ticksPerSecond / ( 1000ll * 1000ll * 1000ll ) );
DECLARE_TIME_TYPE( microseconds_t, g_ticksPerSecond / ( 1000ll * 1000ll ) );
DECLARE_TIME_TYPE( milliseconds_t, g_ticksPerSecond / 1000ll );
DECLARE_TIME_TYPE( seconds_t, g_ticksPerSecond );
#undef DECLARE_TIME_TYPE
