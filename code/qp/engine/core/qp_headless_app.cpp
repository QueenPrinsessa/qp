#include "engine.pch.h"
#include "qp_headless_app.h"
#include "qp/engine/debug/qp_log.h"
#include <iostream>

namespace qp {
    void HeadlessApp::OnInit() {
        debug::Printf( "%s\n", __FUNCTION__ );
    }

    void HeadlessApp::OnBeginUpdate() {
        debug::Printf( "%s\n", __FUNCTION__ );
    }

    void HeadlessApp::OnUpdate() {
        debug::Printf( "%s\n", __FUNCTION__ );
        debug::Printf( "Type 'q' to shutdown.\n" );
        char input = -1;
        std::cin >> input;
        if ( input == 'q' ) {
            RequestShutdown();
        }
    }

    void HeadlessApp::OnEndUpdate() {
        debug::Printf( "%s\n", __FUNCTION__ );
    }

    void HeadlessApp::OnCleanup() {
        debug::Printf( "%s\n", __FUNCTION__ );
    }
}