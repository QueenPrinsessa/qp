#pragma once
#include "Types.h"
#include <limits>

namespace QPEcs
{
	constexpr EntityType MaxEntities = 4096;
	constexpr EntityType NullEntity = std::numeric_limits<EntityType>::max();
	using Entity = EntityType;

}
