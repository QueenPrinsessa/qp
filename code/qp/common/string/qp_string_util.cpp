#include "engine.pch.h"
#include "qp_string_util.h"
#include "qp/common/utilities/qp_algorithms.h"
#include "qp_string.h"

namespace qp {
    namespace string_util {
        char * Duplicate( const char * str ) {
            uint32 len = VerifyStaticCast< uint32 >( StrLen( str ) );
            char * copy = new char[ len + 1 ];
            CopyBytesUnchecked( copy, str, ( len + 1 ) );
            return copy;
        }

        void Free( char * str ) {
            delete[] str;
        }
    }
}