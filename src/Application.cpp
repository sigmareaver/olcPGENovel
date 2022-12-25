#include "Application.h"

Application* Application::m_singleton = nullptr;

Application::Application() :
	PixelGameEngine(),
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

Application& Application::GetInstance()
{
	if (!m_singleton)
		return *(m_singleton = new Application);
	return *m_singleton;
}

void Application::Destroy()
{
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

bool Application::OnUserCreate()
{
	return false;
}

bool Application::OnUserUpdate(float fElapsedTime)
{
	return false;
}

bool Application::OnUserDestroy()
{
	return false;
}

void Application::olc_UpdateMouse(int32_t x, int32_t y)
{
}

void Application::olc_UpdateMouseState(int32_t button, bool state)
{
}

void Application::olc_UpdateMouseWheel(int32_t delta)
{
}

void Application::olc_UpdateKeyState(int32_t key, bool state)
{
}

void Application::olc_UpdateKeyFocus(bool state)
{
}

void Application::olc_UpdateMouseFocus(bool state)
{
}

void Application::olc_UpdateWindowSize(int32_t x, int32_t y)
{
}
