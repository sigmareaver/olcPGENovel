
#pragma once

#include <olcPixelGameEngine/olcPixelGameEngine.h>

#include "Event.h"
#include "FSM.h"
#include "GUI.h"

class MainMenuWindow : public GUI::Window, Event::Listener, olc::PGEX
{
protected:
	olc::Renderable m_main_menu_image;
	bool m_show_confirm_exit;
public:
	MainMenuWindow(const char* title);
	MainMenuWindow(const char* title, ImVec2* position, ImVec2* size, ImGuiWindowFlags flags = 0, bool toggleable = true);
	~MainMenuWindow();

	virtual void OnEventImmediate(Event::Event* event) override;
	virtual bool OnInit() override;
	virtual bool OnGUI(float fElapsedTime) override;
};

class GUILayerMainMenu : public GUI::Layer
{
public:
	virtual bool OnInit() override;
};


class StateMainMenu : public FSM::State, olc::PGEX
{
protected:
	olc::vf2d m_screen[4];
	GUI::Layer* m_gui_layer;
	olc::Renderable m_background;
public:
	StateMainMenu();
	~StateMainMenu();
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float fElapsedTime) override;
	virtual void Render(float fElapsedTime) override;
};

