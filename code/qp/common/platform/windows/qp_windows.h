#pragma once

#if defined( QP_PLATFORM_WINDOWS )

#define NOMINMAX
#define NOCOMM
#define NO_STRICT
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#undef far
#undef near

#define QP_WINDOWS_TYPES_INCLUDED

#endif