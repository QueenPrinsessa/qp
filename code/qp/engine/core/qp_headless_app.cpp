#include "engine.pch.h"
#include "qp_headless_app.h"
#include "qp/engine/debug/qp_log.h"
#include <iostream>

void qpHeadlessApp::OnInit () {
    qpDebug::Printf( "%s\n", __FUNCTION__ );
}

void qpHeadlessApp::OnUpdate () {
    qpDebug::Printf( "%s\n", __FUNCTION__ );
    qpDebug::Printf( "Type 'q' to shutdown.\n" );
    char input = -1;
    std::cin >> input;
    if ( input == 'q' ) {
        RequestShutdown();
    }
}

void qpHeadlessApp::OnCleanup () {
    qpDebug::Printf( "%s\n", __FUNCTION__ );
}
