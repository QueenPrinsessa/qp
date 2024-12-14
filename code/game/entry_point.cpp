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
#include "engine/rendering/qp_render_camera.h"
#include "engine/rendering/qp_render_scene.h"
#include "engine/window/qp_mouse.h"

#define QP_RUN_GAME_UPDATE_JOB( job ) { job _tempJob; _tempJob.Initialize( m_ecs ); threadPool.QueueJob( [ this, &_tempJob ](){ std::scoped_lock lock( m_ecsLock ); _tempJob.Run(); } ); }

namespace qp {
	namespace game {
		class GameApp : public qp::WindowedApp {
		public:
			explicit GameApp( const windowProperties_t & windowProperties )
				: qp::WindowedApp( windowProperties ) {
			}

			struct InputComponent {
				const qp::Keyboard * keyboard;
				const qp::Mouse * mouse;
			};

			virtual void OnInit() override {
				qp::WindowedApp::OnInit();

				threadPool.Startup( threadPool.MaxWorkers() );

				// todo: specialized registry in QPEcs for singletons
				// world entity for holding global data
				m_singletonEntity = m_ecs.CreateEntity();
				m_ecs.AddComponent< UpdateInputComponent >( m_singletonEntity );
				m_ecs.AddComponent< UpdateOutputComponent >( m_singletonEntity );
				m_ecs.AddComponent< InputComponent >( m_singletonEntity ) = InputComponent { &m_window->GetKeyboard(), &m_window->GetMouse() };

				m_window->GetHeight();
				// 
				QPEcs::Entity cameraEntity = m_ecs.CreateEntity();
				m_ecs.AddComponent< qp::TransformComponent >( cameraEntity );
				auto & camera = m_ecs.AddComponent< qp::CameraComponent >( cameraEntity );
				camera.m_hfovDeg = 90.0f;
			}
			virtual void OnUpdate() override {
				SetupGameUpdate();
				RunGameUpdateJobs();
				EndGameUpdate();
			}
			virtual void OnCleanup() override {
				threadPool.Shutdown();
				qp::WindowedApp::OnCleanup();
			}

		private:
			void SetupGameUpdate() {
				// Setup input
				{
					auto & inputView = m_ecs.GetView< UpdateInputComponent >();
					auto & inputComponent = inputView.Get( m_singletonEntity );
					// todo: width / height should come from graphics api / swapchain
					inputComponent.m_renderWidth = m_window->GetClientWidth();
					inputComponent.m_renderHeight = m_window->GetClientHeight();
					inputComponent.m_deltaTime = m_deltaTime.AsSeconds().GetFloat();
				}
			}
			void RunGameUpdateJobs() {
				// todo: something needs to be done to avoid writing to the same data at the same time.
				class CameraUpdateJob : public UpdateJob {
				public:
					virtual void Run() override {
						// hack: get keyboard from world entity which we know is first
						auto & singletonView = m_ecs->GetView< UpdateInputComponent, UpdateOutputComponent, InputComponent >();
						auto [inputComponent, outputComponent, keyboardComponent] = singletonView.Get( singletonView.GetFirstEntity() );
						const qp::Keyboard * keyboard = keyboardComponent.keyboard;
						const qp::Mouse * mouse = keyboardComponent.mouse;

						auto & cameraView = m_ecs->GetView< qp::TransformComponent, qp::CameraComponent >();
						for ( const QPEcs::Entity & entity : cameraView ) {
							auto [transform, camera] = cameraView.Get( entity );

							const float fwdSpeed = 100.0f;
							const float rightSpeed = 100.0f;
							const float upSpeed = 100.0f;
							float forwardDir = 0.0f;
							if ( keyboard->IsKeyDown( qp::keyboardKeys_t::W ) ) {
								forwardDir = 1.0f;
							} else if ( keyboard->IsKeyDown( qp::keyboardKeys_t::S ) ) {
								forwardDir = -1.0f;
							}
							float rightDir = 0.0f;
							if ( keyboard->IsKeyDown( qp::keyboardKeys_t::D ) ) {
								rightDir = 1.0f;
							} else if ( keyboard->IsKeyDown( qp::keyboardKeys_t::A ) ) {
								rightDir = -1.0f;
							}

							float upDir = 0.0f;
							if ( keyboard->IsKeyDown( qp::keyboardKeys_t::Q ) ) {
								upDir = -1.0f;
							} else if ( keyboard->IsKeyDown( qp::keyboardKeys_t::E ) ) {
								upDir = 1.0f;
							}

							qp::Vec2i mousePositionDelta = mouse->GetMouseCursor().GetPositionDelta();
							transform.m_rotation.x += static_cast< float >( mousePositionDelta.y ) * 0.1f;
							transform.m_rotation.y += static_cast< float >( mousePositionDelta.x ) * 0.1f;
							transform.m_orientation = qp::Quat( transform.m_rotation.x, transform.m_rotation.y, transform.m_rotation.z );

							transform.m_translation += transform.m_orientation.Forward() * fwdSpeed * inputComponent.m_deltaTime * forwardDir;
							transform.m_translation += transform.m_orientation.Right() * rightSpeed * inputComponent.m_deltaTime * rightDir;
							transform.m_translation += qp::g_vec3Up * upSpeed * inputComponent.m_deltaTime * upDir;


							const bool debugPrints = false;
							if ( debugPrints ) {
								qp::debug::Printf( "Camera Entity %llu. Translation: %.3f %.3f %.3f.\n", entity, transform.m_translation.x, transform.m_translation.y, transform.m_translation.z );
							}

							outputComponent.m_viewProjection = qp::PerspectiveProjectionMatrix( camera.m_hfovDeg, inputComponent.m_renderWidth, inputComponent.m_renderHeight, camera.m_nearPlane, camera.m_farPlane );
							outputComponent.m_viewTranslation = transform.m_translation;
							outputComponent.m_viewOrientation = transform.m_orientation;
						}
					}
				};

				// Game jobs
				QP_RUN_GAME_UPDATE_JOB( CameraUpdateJob );
				threadPool.WaitForIdle();
			}
			void EndGameUpdate() {
				// Setup camera from output
				{
					auto & outputView = m_ecs.GetView< UpdateOutputComponent >();
					const auto & outputComponent = outputView.Get( m_singletonEntity );
					// todo: width / height should come from graphics api / swapchain
					qp::renderCamera_t renderCamera;
					qp::SetupRenderCamera( renderCamera, outputComponent.m_viewTranslation, outputComponent.m_viewOrientation, outputComponent.m_viewProjection );
					m_renderScene->SetRenderCamera( renderCamera );
				}
			}

			qp::ThreadPool threadPool;
			std::mutex m_ecsLock; // todo: remove once we can schedule multiple jobs and ensure the same component is not written to at the same time.
			QPEcs::Entity m_singletonEntity = QPEcs::NullEntity;
			QPEcs::EntityComponentSystem m_ecs;

		};
	}
}
#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_types_win32.h"
#include "qp/engine/platform/windows/window/qp_window_win32.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {
#if !defined( QP_RETAIL )
	if ( qp::Sys_InitializeConsole() ) {
		qp::debug::Trace( "Successfully initialized console." );
	}
#endif

#if defined( QP_HEADLESS )
	qp::HeadlessApp app;
#else
	qp::windowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = qp::windowMode_t::WINDOWED;
#if defined( QP_VULKAN )
	windowProperties.title = "qpVulkan Window Win32";
#elif defined( QP_D3D11 )
	windowProperties.title = "qpD3D11 Window Win32";
#endif
	qp::windowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	qp::game::GameApp app( windowProperties );
#endif
	
	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qp::debug::Error( "%s", e.what() );
	}

	qp::debug::Trace( "Shutting down." );

	qp::debug::FlushLogFile();
	
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
	qp::HeadlessApp app;
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

	qp::WindowedApp app( windowProperties );
#endif

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qp::debug::Error( "%s", e.what() );
	}

	return 0;
}
#else
#error There's no entrypoint defined for platform!
#endif
