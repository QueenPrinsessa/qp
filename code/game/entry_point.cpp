#include "game.pch.h"
#include "common/threads/qp_thread_pool.h"
#include "jobs/qp_game_update_job.h"
#include "common/jobs/qp_job_system.h"
#if defined( QP_HEADLESS )
#include "qp/engine/core/qp_headless_app.h"
#else
#include "qp/engine/core/qp_windowed_app.h"
#endif

#include "QPEcs.hpp"
#include "qp/common/ecs/components/qp_transform_component.h"
#include "common/ecs/components/qp_camera_component.h"
#include "ecs/components/qp_game_update_input_component.h"
#include "ecs/components/qp_game_update_output_component.h"
#include "engine/window/qp_keyboard.h"
#include "engine/window/qp_window.h"

#define QP_RUN_GAME_UPDATE_JOB( job ) { job _tempJob; _tempJob.Initialize( m_ecs ); threadPool.QueueJob( [ this, &_tempJob ](){ std::scoped_lock lock( m_ecsLock ); _tempJob.Run(); } ); }

class qpGameApp : public qpWindowedApp {
public:
	explicit qpGameApp ( const windowProperties_t & windowProperties )
		: qpWindowedApp( windowProperties ) {
	}
	
	struct qpInputComponent {
		const qpKeyboard * keyboard;
	};

	virtual void OnInit() override {
		qpWindowedApp::OnInit();

		threadPool.Startup( threadPool.MaxWorkers() );
		
		// todo: specialized registry in QPEcs for singletons
		// world entity for holding global data
		m_singletonEntity = m_ecs.CreateEntity();
		m_ecs.AddComponent< qpGameUpdateInputComponent >( m_singletonEntity );
		m_ecs.AddComponent< qpGameUpdateOutputComponent >( m_singletonEntity );
		m_ecs.AddComponent< qpInputComponent >( m_singletonEntity ).keyboard = &m_window->GetKeyboard();

		m_window->GetHeight();
		// 
		QPEcs::Entity cameraEntity = m_ecs.CreateEntity();
		m_ecs.AddComponent< qpTransformComponent >( cameraEntity );
		auto & camera = m_ecs.AddComponent< qpCameraComponent >( cameraEntity );
		camera.m_hfovDeg = 90.0f;
	}
	virtual void OnUpdate() override {
		SetupGameUpdate();
		RunGameUpdateJobs();
		EndGameUpdate();
	}
	virtual void OnCleanup() override {
		threadPool.Shutdown();
		qpWindowedApp::OnCleanup();
	}

private:
	void SetupGameUpdate() {
		// Setup input
		{
			auto & inputView = m_ecs.GetView< qpGameUpdateInputComponent >();
			auto & inputComponent = inputView.Get( m_singletonEntity );
			// todo: width / height should come from graphics api / swapchain
			inputComponent.m_renderWidth = m_window->GetClientWidth();
			inputComponent.m_renderHeight = m_window->GetClientHeight();
			inputComponent.m_deltaTime = m_deltaTime.AsSeconds().GetFloat();
		}
	}
	void RunGameUpdateJobs() {
		// todo: something needs to be done to avoid writing to the same data at the same time.
		class qpCameraGameUpdateJob : public qpGameUpdateJob {
		public:
			virtual void Run() override {
				// hack: get keyboard from world entity which we know is first
				auto & singletonView = m_ecs->GetView< qpGameUpdateInputComponent, qpGameUpdateOutputComponent, qpInputComponent >();
				auto [inputComponent, outputComponent, keyboardComponent] = singletonView.Get( singletonView.GetFirstEntity() );
				const qpKeyboard * keyboard = keyboardComponent.keyboard;

				auto & cameraView = m_ecs->GetView< qpTransformComponent, qpCameraComponent >();
				for ( const QPEcs::Entity & entity : cameraView ) {
					auto [transform, camera] = cameraView.Get( entity );

					const float fwdSpeed = 100.0f;
					const float rightSpeed = 100.0f;
					const float upSpeed = 100.0f;
					float forwardDir = 0.0f;
					if ( keyboard->IsKeyDown( keyboardKeys_t::W ) ) {
						forwardDir = 1.0f;
					} else if ( keyboard->IsKeyDown( keyboardKeys_t::S ) ) {
						forwardDir = -1.0f;
					}
					float rightDir = 0.0f;
					if ( keyboard->IsKeyDown( keyboardKeys_t::D ) ) {
						rightDir = 1.0f;
					} else if ( keyboard->IsKeyDown( keyboardKeys_t::A ) ) {
						rightDir = -1.0f;
					}

					float upDir = 0.0f;
					if ( keyboard->IsKeyDown( keyboardKeys_t::Q ) ) {
						upDir = -1.0f;
					} else if ( keyboard->IsKeyDown( keyboardKeys_t::E ) ) {
						upDir = 1.0f;
					}
					transform.m_translation += transform.m_orientation.Forward() * fwdSpeed * inputComponent.m_deltaTime * forwardDir;
					transform.m_translation += transform.m_orientation.Right() * rightSpeed * inputComponent.m_deltaTime * rightDir;
					transform.m_translation += g_vec3Up * upSpeed * inputComponent.m_deltaTime * upDir;

					const bool debugPrints = false;
					if ( debugPrints ) {
						qpDebug::Printf( "Camera Entity %llu. Translation: %.3f %.3f %.3f.\n", entity, transform.m_translation.x, transform.m_translation.y, transform.m_translation.z );
					}

					outputComponent.m_viewProjection = qpPerspectiveProjectionMatrix( camera.m_hfovDeg, inputComponent.m_renderWidth, inputComponent.m_renderHeight, camera.m_nearPlane, camera.m_farPlane );
					outputComponent.m_viewTranslation = transform.m_translation;
					outputComponent.m_viewOrientation = transform.m_orientation;
				}
			}
		};

		// Game jobs
		QP_RUN_GAME_UPDATE_JOB( qpCameraGameUpdateJob );
		threadPool.WaitForIdle();
	}
	void EndGameUpdate() {
		// Setup camera from output
		{
			auto & outputView = m_ecs.GetView< qpGameUpdateOutputComponent >();
			const auto & outputComponent = outputView.Get( m_singletonEntity );
			// todo: width / height should come from graphics api / swapchain
			qpSetupRenderCamera( m_renderCamera, outputComponent.m_viewTranslation, outputComponent.m_viewOrientation, outputComponent.m_viewProjection );
		}
	}

	qpThreadPool threadPool;
	std::mutex m_ecsLock; // todo: remove once we can schedule multiple jobs and ensure the same component is not written to at the same time.
	QPEcs::Entity m_singletonEntity = QPEcs::NullEntity;
	QPEcs::EntityComponentSystem m_ecs;

};

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_types_win32.h"
#include "qp/engine/platform/windows/window/qp_window_win32.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {
#if !defined( QP_RETAIL )
	if ( Sys_InitializeConsole() ) {
		qpDebug::Trace( "Successfully initialized console." );
	}
#endif

#if defined( QP_HEADLESS )
	qpHeadlessApp app;
#else
	windowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = windowMode_t::WINDOWED;
#if defined( QP_VULKAN )
	windowProperties.title = "qpVulkan Window Win32";
#elif defined( QP_D3D11 )
	windowProperties.title = "qpD3D11 Window Win32";
#endif
	windowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	qpGameApp app( windowProperties );
#endif
	
	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qpDebug::Error( "%s", e.what() );
	}

	qpDebug::Trace( "Shutting down." );

	qpDebug::FlushLogFile();
	
#if !defined( QP_RETAIL )
	// keep console open
	system( "pause" );
#endif
	return 0;
}

#elif defined( QP_PLATFORM_LINUX )
int main() {
#if !defined( QP_RETAIL )
	Sys_InitializeConsole();
#endif

#if defined( QP_HEADLESS )
	qpHeadlessApp app;
#else
	windowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = windowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanLinuxWindow";

	// windowPropertiesWindows_t windowsProperties;
	// windowsProperties.instanceHandle = hInstance;

	//windowProperties.platformData = &windowsProperties;

	qpWindowedApp app( windowProperties );
#endif

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qpDebug::Error( "%s", e.what() );
	}

	return 0;
}
#else
#error There's no entrypoint defined for platform!
#endif
