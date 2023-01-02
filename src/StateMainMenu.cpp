// Internal
#include "Application.h"
#include "Event.h"
#include "Log.h"
#include "Serializer.h"
#include "StateMainMenu.h"

// External
#include <imgui_docking/imgui_internal.h>
#include <filesystem>

// GUI::Window implementation
MainMenuWindow::MainMenuWindow(const char* title) :
	Window(title),
	Listener(),
	PGEX(),
	m_show_confirm_exit{false}
{
	LOG("MainMenuWindow: Loading resources...");
	std::filesystem::path path = Serializer::IMAGE_PATH / "mainmenu.png";
	m_main_menu_image.Load(path.string().c_str());
}

MainMenuWindow::MainMenuWindow(const char* title, ImVec2* position, ImVec2* size, ImGuiWindowFlags flags, bool toggleable) :
	Window(title, position, size, flags, toggleable),
	Listener(),
	PGEX(),
	m_show_confirm_exit{false}
{
	LOG("MainMenuWindow: Loading resources...");
	std::filesystem::path path = Serializer::IMAGE_PATH / "mainmenu.png";
	m_main_menu_image.Load(path.string().c_str());
}

MainMenuWindow::~MainMenuWindow()
{
	//m_event_handler.UnregisterListener(this, Event::Type::CLOSE, true);
}

void MainMenuWindow::OnEventImmediate(Event::Event* event)
{
	if (event->type == Event::Type::CLOSE)
	{
		m_show_confirm_exit = true;
	}
}

bool MainMenuWindow::OnInit()
{
	LOG("MainMenuWindow: Setting up event listener...");
	//m_event_handler.RegisterListener(this, Event::Type::CLOSE, true);
	RegisterEvent(Event::Type::CLOSE, true);

	m_show_window = true;

	return true;
}

bool MainMenuWindow::OnGUI(float fElapsedTime)
{
	bool ret_val = true;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, { 1.000f, 0.729f, 0.718f, 0.000f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.000f, 0.729f, 0.718f, 1.000f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1.000f, 1.000f, 1.000f, 1.000f });

	ImGui::PushID(10000);
	if (ImGui::ImageButton("NewGame", (void*)(intptr_t)m_main_menu_image.Decal()->id,
		{ 59 * 4, 15 * 4 },
		{ 0, 0 }, { 59.0f / 128.0f, 15.0f / 128.0f }))
	//ImGui::Button("New Game");
	{
		//if (ImGui::GetIO().KeyMods & ImGuiKeyModFlags_Shift)
		//{
		//	Event::FSMTransition<StateDeveloperMode>(true);
		//}
		//else
		//{
		//	Event::FSMTransition<StateNewGame>(true);
		//}
	}
	ImGui::PopID();
	ImGui::PushID(10001);
	//static ImVec2 uv1, uv2;
	if (ImGui::ImageButton("LoadGame", (void*)(intptr_t)m_main_menu_image.Decal()->id,
		{ 64 * 4, 15 * 4 },
		{ 0.0f, 19.0f / 128.0f }, { 64.0f / 128.0f, 34.0f / 128.0f }))
	//ImGui::Button("Load Game");
	{
		//Event::FSMTransition<StateLoadGame>(true);
		//Event::Event* event = new Event::Event(Event::Type::FSM_TRANSITION);
		//event->AddParam(new StateLoadGame);
		//Event::Handler::GetInstance() << event;
	}
	/*if (ImGui::IsWindowHovered() && ImGui::IsItemHovered())
	{
		uv1 = { 0.0f, 19.0f / 128.0f };
		uv2 = { 64.0f / 128.0f, 34.0f / 128.0f };
	}
	else
	{
		uv1 = { 0.0f, 38.0f / 128.0f };
		uv2 = { 47.0f / 128.0f, 57.0f / 128.0f };
	}*/
	ImGui::PopID();
	ImGui::PushID(10002);
	ImGui::ImageButton("Options", (void*)(intptr_t)m_main_menu_image.Decal()->id,
		{ 47 * 4, 19 * 4 },
		{ 0.0f, 38.0f / 128.0f }, { 47.0f / 128.0f, 57.0f / 128.0f });
	//ImGui::Button("Options");
	ImGui::PopID();
	ImGui::PushID(10003);
	if (ImGui::ImageButton("QuitGame", (void*)(intptr_t)m_main_menu_image.Decal()->id,
		{ 26 * 4, 18 * 4 },
		{ 0.0f, 58.0f / 128.0f }, { 26.0f / 128.0f, 76.0f / 128.0f }))
	//if (ImGui::Button("Quit Game"))
	{
		Event::Close(false);
		//m_show_confirm_exit = true;
	}
	ImGui::PopID();

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
	if (m_show_confirm_exit)
	{
		m_show_confirm_exit = false;
		if (!ImGui::IsPopupOpen("Confirm Exit"))
			ImGui::OpenPopup("Confirm Exit");
	}
	if (ImGui::BeginPopupModal("Confirm Exit", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Are you sure you want to quit?");

		// Button centering hack
		ImGuiStyle& style = ImGui::GetStyle();
		float size = ImGui::CalcItemSize({ 75.0f, 0.0f }, 0, 0).x * 2.0f + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;
		float off = (avail - size) * 0.5f;
		float width = ImGui::GetContentRegionAvail().x - ImGui::CalcItemWidth();
		ImGui::Indent(off);

		if (ImGui::Button("Exit", { 75.0f, 0.0f }))
			ret_val = false;
		ImGui::SameLine();
		ImGui::SetItemDefaultFocus();
		if (ImGui::Button("Cancel", { 75.0f, 0.0f }))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	return ret_val;
}


// GUI::Layer implementation
bool GUILayerMainMenu::OnInit()
{
	AddWindow(new MainMenuWindow("Main Menu",
		new ImVec2(100.0f, 100.0f),
		new ImVec2(300.0f, 400.0f),
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings,
		false
	));
	//AddWindow(new GUI::StyleEditor());

	//std::filesystem::path path = Serializer::RES_DIR; path = path / Serializer::LUA_DIR / Serializer::GUI_DIR / "test_window.lua";
	//AddWindow(new GUI::LuaWindow(*Lua::Engine::LoadFile(path), "Test Window"));
	//ToggleWindow("Test Window");

	//auto editor = new GUI::TextEditor("Text Editor");
	//editor->LoadFile(path);
	//AddWindow(editor);

	return true;
}


// FSM::State implementation
StateMainMenu::StateMainMenu()
{
	LOG("Initializing main menu...");

	LOG("Loading resources...");
	std::filesystem::path path = Serializer::IMAGE_PATH / "test.jpg";
	m_background.Load(path.string());

	LOG("Setting up GUI layers...");
	m_gui_layer = new GUILayerMainMenu;
	m_gui_layer->Initialize();
	//GUI::Manager::GetInstance().SetupLayers();
}

StateMainMenu::~StateMainMenu()
{
	delete m_gui_layer;
}

void StateMainMenu::OnEnter()
{
	GUI::Manager::GetInstance().PushLayer(m_gui_layer);
}

void StateMainMenu::OnExit()
{
	GUI::Manager::GetInstance().PopLayer();
}

void StateMainMenu::Update(float fElapsedTime)
{
	static const float image_aspect = float(m_background.Sprite()->width) / float(m_background.Sprite()->height);
	float screen_aspect = float(pge->ScreenWidth()) / float(pge->ScreenHeight());
	if (screen_aspect >= image_aspect)
	{
		m_screen[0] = { (1.0f - (image_aspect / screen_aspect)) * float(pge->ScreenWidth()) / 2.0f, 0.0f };
		m_screen[1] = { float(pge->ScreenWidth()) - m_screen[0].x, 0.0f };
		m_screen[2] = { m_screen[1].x, float(pge->ScreenHeight()) };
		m_screen[3] = { m_screen[0].x, m_screen[2].y };
	}
	else
	{
		m_screen[0] = { 0.0f, (1.0f - (screen_aspect / image_aspect)) * float(pge->ScreenHeight()) / 2.0f };
		m_screen[1] = { float(pge->ScreenWidth()), m_screen[0].y };
		m_screen[2] = { m_screen[1].x, float(pge->ScreenHeight()) - m_screen[0].y };
		m_screen[3] = { 0.0f, m_screen[2].y };
	}
}

void StateMainMenu::Render(float fElapsedTime)
{
	static olc::vf2d uv[] = {
		{0.0, 0.0},
		{1.0, 0.0},
		{1.0, 1.0},
		{0.0, 1.0}
	};
	static olc::Pixel color[] = {
		olc::WHITE, olc::WHITE, olc::WHITE, olc::WHITE
	};
	pge->SetDrawTarget(Application::GameLayer(), false);
	pge->DrawExplicitDecal(m_background.Decal(), m_screen, uv, color, 4);
}
