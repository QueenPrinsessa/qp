#include "engine/qp_app.h"
#include "tools/string/qp_string.h"
#include <iostream>

int main () {

	class qpTestApp : public qpApp {
	public:
		virtual void OnInit () override {

		}

		virtual void OnUpdate () override {
			//std::cout << __FUNCTION__ << std::endl;
		}

		virtual void OnCleanup () override {

		}
	};

	qpTestApp app;

	try {
		app.Run ();
	} catch ( const std::exception & e ) {
		std::cerr << e.what () << std::endl;
	}

	return 0;
}