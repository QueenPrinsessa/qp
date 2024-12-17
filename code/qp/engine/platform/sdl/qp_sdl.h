#pragma once

#if defined( QP_PLATFORM_SDL )
#include <SDL3/SDL.h>

namespace qp {
	extern bool InitializeSDL();
	extern void ShutdownSDL();
}
#endif