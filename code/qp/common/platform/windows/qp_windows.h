#pragma once

#if defined( QP_PLATFORM_WINDOWS )

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define WIN
#include "Windows.h"

#undef near
#undef far

#endif