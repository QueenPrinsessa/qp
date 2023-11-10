#include "qp/common/time/qp_time_point.h"
#ifdef QP_PLATFORM_WINDOWS
#include "qp/common/time/qp_clock.h"
#include "qp/common/core/qp_types.h"
#include "qp/common/platform/windows/qp_windows.h"

qpTimePoint qpClock::Now() {
    static LARGE_INTEGER frequency {};
    static BOOL useHighResolution = QueryPerformanceFrequency( &frequency );
    static_cast< void >( useHighResolution );
    LARGE_INTEGER now {};
    QueryPerformanceCounter( &now );
    int64 timeNs = ( 1000ll * 1000ll * now.QuadPart ) / frequency.QuadPart;
    return qpTimePoint{ now.QuadPart, frequency.QuadPart };
}

#endif
