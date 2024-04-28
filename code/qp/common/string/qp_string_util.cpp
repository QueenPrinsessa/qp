#include "engine.pch.h"
#include "qp_string_util.h"
#include "qp/common/utilities/qp_algorithms.h"
#include "qp_string.h"

namespace qpStringUtil {
    char * Duplicate( const char * str ) {
        uint32 len = qpVerifyStaticCast< uint32 >( qpStrLen( str ) );
        char * copy = new char[ len + 1 ];
        qpCopyBytesUnchecked( copy, str, ( len + 1 ) );
        return copy;
    }

    void Free( char * str ) {
        delete[] str;
    }
}