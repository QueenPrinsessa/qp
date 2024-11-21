#pragma once
#include "Types.h"
#include <bitset>

namespace QPEcs
{
	constexpr ComponentType MaxComponents = 64;

	using Signature = std::bitset<MaxComponents>;
}
