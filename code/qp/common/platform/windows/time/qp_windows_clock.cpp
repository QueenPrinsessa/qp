#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/core/qp_macros.h"
#include "qp/common/time/qp_time_point.h"
#include "qp/common/time/qp_clock.h"
#include "qp/common/platform/windows/qp_windows.h"

qpTimePoint qpClock::Now() {
    LARGE_INTEGER now {};
    QueryPerformanceCounter( &now );
    return qpTimePoint{ now.QuadPart, TicksPerSecond() };
}

timeTick_t qpClock::TicksPerSecond() {
    static LARGE_INTEGER frequency {};
    static BOOL useHighResolution = QueryPerformanceFrequency( &frequency );
    QP_DISCARD_RESULT useHighResolution;
    return frequency.QuadPart;
}

#endif
