#pragma once

// Submodules
#include "olcPixelGameEngine/olcPixelGameEngine.h"

// Internals
#include "Lua.h"

// Externals
#include <imgui_docking/imgui.h>
#include <list>


#define SCREENWIDTH		1280
#define SCREENHEIGHT	720


namespace GUI
{
	class Window
	{
	protected:
		bool m_initialized;
		char* m_window_title;
		ImVec2* m_position;
		bool m_fixed_xpos;
		bool m_fixed_ypos;
		ImVec2* m_size;
		bool m_fixed_width;
		bool m_fixed_height;
		ImGuiWindowFlags m_window_flags;
		bool m_toggleable;
		bool m_show_window;
		ImFont* m_font;

		virtual void SetWindowParams(float fElapsedTime);
	public:
		Window(const char* const title);
		Window(const char* const title, ImVec2* position, ImVec2* size, ImGuiWindowFlags flags = 0, bool toggleable = true);
		virtual ~Window();

		virtual bool OnInit();
		virtual bool OnGUI(float fElapsedTime) = 0;
		virtual bool OnDestroy();

		bool Initialize();
		virtual bool DrawWindow(float fElapsedTime);

		bool SetFont(const char* const key);
		void ClearFont();
		void ShowWindow(bool show);
		const char* Name();
		bool Visible();
	};

	class Layer
	{
	protected:
		bool m_initialized;
		std::list<Window*> m_window_list;
		bool m_show_layer;
	public:
		Layer();
		virtual ~Layer();

		virtual bool OnInit();
		virtual bool OnDestroy();

		virtual bool DrawLayer(float fElapsedTime);
		bool Initialize();

		void AddWindow(Window* window);
		void EraseWindow(Window* window);
		bool ToggleWindow(const char* const name);

		void ShowLayer(bool show);
		bool Visible();
	};

	class Manager : public olc::PGEX
	{
	private:
		static Manager* m_singleton;
	protected:
		Lua::Engine& m_lua;
		std::map<const std::string, ImFont*> m_font_map;
		std::list<Layer*> m_layer_stack;
		bool m_running;
		float m_scroll_sensitivity = 1.0f;
	protected:
		Manager();
	public:
		static inline auto& GetInstance() { if (!m_singleton) m_singleton = new Manager; return *m_singleton; }
		static inline auto& Lua() { return GetInstance().m_lua; }
		static ImFont* GetFont(const char* const key);
		static bool ToggleWindow(const char* const name);
		~Manager();

		bool Initialize();
		bool SetupLayers();
		bool DrawGUI(float fElapsedTime);
		void Render();
		bool Shutdown();

		void PushLayer(Layer* layer);
		bool PopLayer();

		void UpdateKeyModifiers(int mods);
		void KeyCallback(int32_t key, bool state, int mods);
		void CursorPosCallback(HWND hwnd, int32_t x, int32_t y);
		void MouseButtonCallback(HWND hwnd, int button, bool state, int mods);
		void ScrollCallback(int32_t xoffset, int32_t yoffset);
		void WindowFocusCallback(bool focused);
		void MouseLeaveCallback(HWND hwnd, bool state);
		void CharCallback(unsigned int c);
	};

	void MDText(const char* text);

	static ImGuiKey KeyToImGuiKey(int32_t key);
	unsigned int KeyToChar(int32_t key, bool shift, bool capslock);

	static void StyleColors();

	float SetNextItemWidth(float percent, float min, float max);

	bool IsShortcutPressed(ImGuiKey mods, ImGuiKey key/*, ImGuiID owner_id = 0*/);
}
