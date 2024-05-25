#include "engine.pch.h"
#include "qp_headless_app.h"
#include "qp/engine/debug/qp_log.h"
#include <iostream>

void qpHeadlessApp::OnInit () {
    qpLog::Printf( "%s\n", __FUNCTION__ );
}

void qpHeadlessApp::OnUpdate () {
    qpLog::Printf( "%s\n", __FUNCTION__ );
    qpLog::Printf( "Type 'q' to shutdown.\n" );
    char input = -1;
    std::cin >> input;
    if ( input == 'q' ) {
        RequestShutdown();
    }
}

void qpHeadlessApp::OnCleanup () {
    qpLog::Printf( "%s\n", __FUNCTION__ );
}
