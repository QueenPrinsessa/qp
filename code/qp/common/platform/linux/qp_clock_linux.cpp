#include "engine.pch.h"

//#if defined( QP_PLATFORM_LINUX )

#include "qp/common/core/qp_macros.h"
#include "qp/common/time/qp_time_point.h"
#include "qp/common/time/qp_clock.h"

namespace qp {
    TimePoint Clock::Now() {
        timespec now;
        QP_VERIFY( clock_gettime( CLOCK_MONOTONIC_RAW, &now ) == 0 );
        const timeTick_t ticksPerSec = TicksPerSecond();
        const timeTick_t ticksPerNanoSec = ( 1'000'000'000 / ticksPerSec );
        const timeTick_t nowTicks = now.tv_sec * ticksPerSec + now.tv_nsec / ticksPerNanoSec;
        return TimePoint { nowTicks, ticksPerSec };
    }

    static int64 frequency = -1;
    timeTick_t Clock::TicksPerSecond() {
        if ( frequency == -1 ) {
            timespec resolution;
            QP_VERIFY( clock_getres( CLOCK_MONOTONIC_RAW, &resolution ) == 0 );
            QP_ASSERT_RELEASE( resolution.tv_sec == 0 && resolution.tv_nsec > 0 ); // resolution worse than a second?
            frequency = 1'000'000'000 / resolution.tv_nsec;
        }
        return frequency;
    }
}
//#endif
