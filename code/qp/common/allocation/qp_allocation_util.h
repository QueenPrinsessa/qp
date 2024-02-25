#pragma once
#include "qp/common/core/qp_types.h"

namespace qpAllocationUtil {
	constexpr static uint64 AlignSize( const uint64 size, const uint64 alignment ) {
		if ( size < alignment ) {
			return alignment;
		}

		return size + ( alignment - ( size % alignment ) ) % alignment;
	}
};
