#include "engine.pch.h"
#include "qp_app.h"

qpApp::qpApp() {
}

qpApp::~qpApp() {
}

void qpApp::RequestShutdown() {
	m_isRunning = false;
}

void qpApp::Run() {
	m_isRunning = true;

	OnInit();

	while ( m_isRunning ) {
		OnBeginFrame();
		OnUpdate();
		OnEndFrame();
	}

	OnCleanup();
}
