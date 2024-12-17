#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/core/qp_macros.h"
#include "qp/common/time/qp_time_point.h"
#include "qp/common/time/qp_clock.h"
#include "qp/common/platform/windows/qp_windows.h"

namespace qp {
    TimePoint Clock::Now() {
        LARGE_INTEGER now {};
        QueryPerformanceCounter( &now );
        return TimePoint { now.QuadPart, TicksPerSecond() };
    }

    static LARGE_INTEGER frequency = { .QuadPart = -1 };
    timeTick_t Clock::TicksPerSecond() {
        if ( frequency.QuadPart == -1 ) {
            QP_VERIFY( QueryPerformanceFrequency( &frequency ) );
        }
        QP_ASSERT( frequency.QuadPart > 0 );
        return frequency.QuadPart;
    }
}
#endif
