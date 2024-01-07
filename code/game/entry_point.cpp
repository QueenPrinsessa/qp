#include "game.pch.h"
#include "common/string/qp_string.h"
#include <chrono>
#include <iostream>
#include "engine/core/qp_app.h"
#include "engine/core/qp_windowed_app.h"
#include <common/math/qp_mat.h>
#include <common/utilities/qp_variant.h>

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/platform/windows/window/qp_window_windows.h"
#include <variant>

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow ) {
	qpWindowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = qpWindowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanWindowsWindow";

	qpWindowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	struct type_t {
		type_t() {
			printf( "hello 2 :>" );
		}
		~type_t() {
			printf( "hello :>" );
		}
		char a[ 213678 ] {};
	};

	qpVariant< int, float, double, type_t > variant;
	int x = sizeof( variant );
	( void ) x;
	variant.Emplace< double >( 5.0 );

	double value = variant.GetValue< double >();
	( void ) value;
	variant.Emplace< type_t >();
	type_t t = variant.GetValue< type_t >();
	variant.Emplace< float >( 23.0f );
	float val = variant.GetValue< float >();
	( void ) val;
	std::variant< int, float, double, type_t > abc;
	abc.emplace< double >( 5.0 );
	
	qpWindowedApp app( windowProperties );

	qpMat4 mat;
	mat[ 2 ][ 3 ] = 4.0f;

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

#endif
