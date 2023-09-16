#pragma once
#include "qp/common/core/qp_types.h"

class qpAllocationUtil {
public:
	constexpr static uint64 AlignSize( uint64 size, uint64 alignment ) {
		if ( size < alignment ) {
			return alignment;
		}

		return size + ( alignment - ( size % alignment ) ) % alignment;
	}
};
