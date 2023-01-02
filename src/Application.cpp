// Internal
#include "Application.h"
#include "Log.h"
#include "PlatformUtils.h"
#include "StateMainMenu.h"

// External
#include <imgui_docking/imgui.h>
#include <imgui_docking/imgui_internal.h>
#include <imgui_docking/backends/imgui_impl_win32.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

Application* Application::m_singleton = nullptr;

Application::Application() :
	PixelGameEngine(),
	Listener(),
	m_event_handler(Event::Handler::GetInstance()),
	m_gui_manager(GUI::Manager::GetInstance()),
	m_lua_engine(Lua::Engine::GetInstance()),
	m_running(false),
	m_game_layer(0),
	m_gui_layer(0),
	m_cutscene_layer(0),
	m_update_screen(0)
{
}

Application::~Application()
{
}

void Application::Destroy()
{
	if (m_singleton)
	{
		delete m_singleton;
		m_singleton = nullptr;
	}
}

uint8_t Application::CutsceneLayer()
{
	return GetInstance().m_cutscene_layer;
}

uint8_t Application::GUILayer()
{
	return GetInstance().m_gui_layer;
}

uint8_t Application::GameLayer()
{
	return GetInstance().m_game_layer;
}

Config::ConfigData& Application::Config()
{
	return GetInstance().m_config;
}

FSM::State* Application::CurrentState()
{
	return GetInstance().m_state_machine.CurrentState();
}

void Application::OnEventImmediate(Event::Event* event)
{
	if (event->type == Event::Type::CLOSE)
	{
		bool fquit = *static_cast<bool*>((*event->params.begin()).value);
		if (fquit)
			m_running = false;
		return;
	}
	if (event->type == Event::Type::SET_KEYBIND)
	{
		auto it = event->params.begin();
		uint32_t key = *static_cast<uint32_t*>((*it).value);
		it++;
		m_key_bindings[key] = *static_cast<std::function<void(int)>*>((*it).value);
		return;
	}
}

void Application::Construct(Config::ConfigData& config)
{
	m_config = std::move(config);
	PixelGameEngine::Construct(m_config["ScreenWidth"], m_config["ScreenHeight"], 1, 1, m_config["Fullscreen"], m_config["VSync"]);
}

bool Application::OnUserCreate()
{
	
	// Setup PGE
	sAppName = "olcPixelNovelEngine v0.0.1";
	LOG("{}, PixelGameEngine {}", sAppName, PGE_VER);
	SetPixelMode(olc::Pixel::Mode::ALPHA);

	// Need separate layer for game elements
	LOG("Creating render layers...");
	m_cutscene_layer = 0;
	m_gui_layer = CreateLayer();
	m_game_layer = CreateLayer();
	EnableLayer(m_gui_layer, true);
	EnableLayer(m_game_layer, true);
	SetDrawTarget(m_cutscene_layer);
	Clear(olc::BLANK);
	SetDrawTarget(m_gui_layer);
	Clear(olc::BLANK);
	SetDrawTarget(m_game_layer);
	Clear(olc::BLANK);

	// Setup event handler (if needed, eventually)
	LOG("Setting up event handler...");
	// m_event_handler

	// Setup event listener
	LOG("Registering application event listeners...");
	m_event_handler.RegisterListener(this, Event::Type::CLOSE, true);
	m_event_handler.RegisterListener(this, Event::Type::SET_KEYBIND, true);
	//RegisterEvent(Event::Type::CLOSE, true);
	//RegisterEvent(Event::Type::SET_KEYBIND, true);

	// Setup GUI
	LOG("Initializing GUI...");
	m_gui_manager.Initialize(); // Call gui manager init after adding all windows/layers, so that their OnInit functions are called.

	// Setup FSM
	LOG("Initializing finite state machine...");
	m_state_machine.Initialize();

	// Bind Keys
	/*m_key_bindings[olc::ESCAPE] = [&](int mods) {
		Event::Close(false);
	};
	m_key_bindings[olc::ENTER] = [&](int mods) {
		if (g_ch->in_room)
			Log(Channel::Local, "[{}]\n {}", g_ch->in_room->name, g_ch->in_room->description);
	};
	m_key_bindings[olc::NP1] = [&](int mods) {
		DoSouthwest(g_ch);
	};
	m_key_bindings[olc::NP2] = [&](int mods) {
		DoSouth(g_ch);
	};
	m_key_bindings[olc::NP3] = [&](int mods) {
		DoSoutheast(g_ch);
	};
	m_key_bindings[olc::NP4] = [&](int mods) {
		DoWest(g_ch);
	};
	m_key_bindings[olc::NP6] = [&](int mods) {
		DoEast(g_ch);
	};
	m_key_bindings[olc::NP7] = [&](int mods) {
		DoNorthwest(g_ch);
	};
	m_key_bindings[olc::NP8] = [&](int mods) {
		DoNorth(g_ch);
	};
	m_key_bindings[olc::NP9] = [&](int mods) {
		DoNortheast(g_ch);
	};*/

	LOG("Pushing StateMainMenu...");
	//ImGui_ImplOpenGL2_NewFrame(); // A slight hack to make ImGui::GetMainViewport() work during GUI layer setup...
	//ImGui::NewFrame();
	//m_state_machine.Push(new StateMainMenu());
	Event::FSMPush<StateMainMenu>();
	//ImGui::EndFrame();
	//ImGui::UpdatePlatformWindows();

	LOG("Application initialization complete...");
	m_running = true;

	return true;
}

bool Application::OnUserUpdate(float fElapsedTime)
{
	try
	{
		if (m_running)
		{
			// Check for screen resize, update accordingly
			if (m_update_screen > 0)
			{
				m_update_screen--;
				SetScreenSize(GetWindowSize().x, GetWindowSize().y);
				SetDrawTarget(m_cutscene_layer);
				Clear(olc::BLANK);
				SetDrawTarget(m_gui_layer);
				Clear(olc::BLANK);
				SetDrawTarget(m_game_layer);
				Clear(olc::BLANK);
			}

			// Setup a new ImGui frame
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = { static_cast<float>(GetWindowSize().x), static_cast<float>(GetWindowSize().y) };
			io.DeltaTime = fElapsedTime != 0.0f ? fElapsedTime : 0.0001f; // Workaround because first frame may have a delta time of 0.0
			ImGui_ImplOpenGL3_NewFrame();
			//ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// Process the current game state
			m_state_machine.Update(fElapsedTime);
			// Process non-immediate mode event listeners
			m_event_handler.ProcessEvents();
			// Process the GUI
			m_running = m_gui_manager.DrawGUI(fElapsedTime);
			// Render the current game state
			m_state_machine.Render(fElapsedTime);

			ImGui::EndFrame();
		}
	}
	catch (const std::exception& e)
	{
		LOG("Caught Exception: {}", e.what());
		return false;
	}

	return m_running;
}

bool Application::OnUserDestroy()
{
	if (m_running)
	{
		Event::Close(false);
		return false;
	}

	m_event_handler.ProcessEvents();
	m_state_machine.Exit();
	m_gui_manager.Shutdown();

	return true;
}

void Application::olc_UpdateMouse(int32_t x, int32_t y)
{
	PixelGameEngine::olc_UpdateMouse(x, y);
	m_gui_manager.CursorPosCallback(nullptr /*(HWND)last_hwnd*/, x, y);
}

void Application::olc_UpdateMouseState(int32_t button, bool state)
{
	PixelGameEngine::olc_UpdateMouseState(button, state);
	int mods = (int(GetKey(olc::Key::SHIFT).bHeld) << 1) | int(GetKey(olc::Key::CTRL).bHeld);
	m_gui_manager.MouseButtonCallback(nullptr /*(HWND)last_hwnd*/, button, state, mods);
}

void Application::olc_UpdateMouseWheel(int32_t delta)
{
	PixelGameEngine::olc_UpdateMouseWheel(delta);
	m_gui_manager.ScrollCallback(0, delta);
}

void Application::olc_UpdateKeyState(int32_t key, bool state)
{
	PixelGameEngine::olc_UpdateKeyState(key, state);
	int mods = (int(GetKey(olc::Key::SHIFT).bHeld) << 1) | int(GetKey(olc::Key::CTRL).bHeld);
	auto imgui_context = ImGui::GetCurrentContext();
	if (!imgui_context || !ImGui::GetIO().WantTextInput)
	{
		if (state && m_key_bindings.find(key) != m_key_bindings.end())
			m_key_bindings[key](mods);
	}
	if (imgui_context)
	{
		m_gui_manager.KeyCallback(key, state, mods);
		if (state && (mods == 0 || mods == 2))
		{
			unsigned int character = GUI::KeyToChar(key, GetKey(olc::Key::SHIFT).bHeld, Utils::GetCapsLockState());
			if (character != '\0')
				m_gui_manager.CharCallback(character);
		}
	}
}

void Application::olc_UpdateKeyFocus(bool state)
{
	PixelGameEngine::olc_UpdateKeyFocus(state);
	m_gui_manager.WindowFocusCallback(state);
}

void Application::olc_UpdateMouseFocus(bool state)
{
	PixelGameEngine::olc_UpdateMouseFocus(state);
	m_gui_manager.MouseLeaveCallback(nullptr /*(HWND)last_hwnd*/, state);
}

void Application::olc_UpdateWindowSize(int32_t x, int32_t y)
{
	PixelGameEngine::olc_UpdateWindowSize(x, y);
	m_update_screen = 2; // Update for 2 frames, since sometimes it misses an update
}
