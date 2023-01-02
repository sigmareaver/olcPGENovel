#pragma once

// Submodules
#include "olcPixelGameEngine/olcPixelGameEngine.h"

// Internals
#include "ConfigData.h"
#include "Event.h"
#include "FSM.h"
#include "GUI.h"
#include "Lua.h"

class Application : public olc::PixelGameEngine, Event::Listener
{
private:
	static Application* m_singleton;
	Application();
protected:
	Config::ConfigData m_config;

	Event::Handler& m_event_handler;
	FSM::Machine m_state_machine;
	GUI::Manager& m_gui_manager;
	Lua::Engine& m_lua_engine;
	std::map<uint32_t, std::function<void(int)>> m_key_bindings;

	uint8_t m_game_layer;
	uint8_t m_gui_layer;
	uint8_t m_cutscene_layer;

	bool m_running;
	int m_update_screen; // Workaround: Using int instead of bool to call the update multiple times, because sometimes it was missing an update.
public:
	~Application();

	static inline Application& GetInstance() { if (!m_singleton) m_singleton = new Application; return *m_singleton; }
	static void Destroy();

	static uint8_t CutsceneLayer();
	static uint8_t GUILayer();
	static uint8_t GameLayer();

	static Config::ConfigData& Config();
	static FSM::State* CurrentState();

	virtual void OnEventImmediate(Event::Event* event) override;

	void Construct(Config::ConfigData& config);
	virtual bool OnUserCreate() override;
	virtual bool OnUserUpdate(float fElapsedTime) override;
	virtual bool OnUserDestroy() override;

	virtual void olc_UpdateMouse(int32_t x, int32_t y) override;
	virtual void olc_UpdateMouseState(int32_t button, bool state) override;
	virtual void olc_UpdateMouseWheel(int32_t delta) override;
	virtual void olc_UpdateKeyState(int32_t key, bool state) override;
	virtual void olc_UpdateKeyFocus(bool state) override;
	virtual void olc_UpdateMouseFocus(bool state) override;
	virtual void olc_UpdateWindowSize(int32_t x, int32_t y) override;
};
