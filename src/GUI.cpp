// Submodules
#include "imgui_md/imgui_md.h"

// Internals
#include "Application.h"
#include "GUI.h"
#include "Serializer.h"
#include "StringHashMap.h"

// Externals
#include <imgui_docking/imgui.h>
#include <imgui_docking/imgui_internal.h>
#include <imgui_docking/backends/imgui_impl_win32.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>
#include <imgui_docking/misc/freetype/imgui_freetype.h>
#include <filesystem>

/*ImGui_ImplWin32_Data* ImGui_ImplWin32_GetBackendData();
bool IsVkDown(int vk);*/
static bool IsVkDown(int vk)
{
    return (::GetKeyState(vk) & 0x8000) != 0;
}

namespace GUI
{
    Manager* Manager::m_singleton = nullptr;

    //////////////////////////////////
    /// GUI values
    //////////////////////////////////
    const ImVec4 LayoutPadding = { 4.0f, 4.0f, 4.0f, 4.0f };


    //////////////////////////////////
    /// Manager class
    //////////////////////////////////
    Manager::Manager() : 
        m_lua{ Lua::Engine::GetInstance() },
        PGEX()
    {
        m_running = false;
    }

    Manager::~Manager()
    {
    }

    ImFont* Manager::GetFont(const char* const key)
    {
        auto& instance = GetInstance();
        auto it = instance.m_font_map.find(key);
        if (it != instance.m_font_map.end())
            return it->second;
        return nullptr;
    }

    bool Manager::ToggleWindow(const char* const name)
    {
        if (!m_singleton)
            return false;
        /*for (auto it = m_singleton->m_layer_stack.begin(); it != m_singleton->m_layer_stack.end(); it++)
        {
            if ((*it)->ToggleWindow(name))
                return true;
        }*/
        if (!m_singleton->m_layer_stack.empty() && m_singleton->m_layer_stack.back()->ToggleWindow(name))
            return true;
        return false;
    }


    bool Manager::Initialize()
    {
	    IMGUI_CHECKVERSION();
	    ImGui::CreateContext();
	    ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { static_cast<float>(pge->GetWindowSize().x), static_cast<float>(pge->GetWindowSize().y) };
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

	    pge->SetLayerCustomRenderFunction(Application::GUILayer(), std::bind(&Manager::Render, this));

	    // Assign GUI hotkeys here

	    // Until we have our own theme...
	    //ImGui::StyleColorsDark();
	    ImGui::StyleColorsClassic();
        //ImGui::StyleColorsLight();
        //StyleColors();

	    // Using OpenGL3
        //ImGui_ImplWin32_Init((HWND)olc::platform->platform_handle, (HGLRC)olc::renderer->renderer_handle);
	    ImGui_ImplOpenGL3_Init();

	    // Load custom fonts here
        //_LoadFonts();
        //io.Fonts->AddFontDefault();
        /*std::filesystem::path path = std::filesystem::current_path() / Serializer::RES_DIR / Serializer::FONT_DIR / "queen-mab.otf";
        {
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 1.0f, 0.0f };
            //ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
            m_font_map["regular"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 16.0f, &ifc);
        }
        {
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 0.0f, 0.0f };
            //ifc.GlyphOffset = { -1.0f, -1.0f };
            //ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
            ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Bold;
            m_font_map["bold"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 16.0f, &ifc);
        }
        {
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 1.0f, 0.0f };
            //ifc.GlyphOffset = { -1.0f, -1.0f };
            //ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
            ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Oblique;
            m_font_map["italic"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 16.0f, &ifc);
        }
        {
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 0.0f, 0.0f };
            //ifc.GlyphOffset = { -1.0f, -1.0f };
            //ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;
            ifc.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_Oblique;
            m_font_map["bold_italic"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 16.0f, &ifc);
        } */
        /*{
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 1.0, 0.0 };
            m_font_map["queenmabH3"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 20.0f, &ifc);
        }*/
        /*{
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 1.0f, 0.0f };
            m_font_map["H2"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 22.0f, &ifc);
        }
        {
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 1.0f, 0.0f };
            m_font_map["H1"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 28.0f, &ifc);
        }
        {
            path = std::filesystem::current_path() / Serializer::RES_DIR / Serializer::FONT_DIR / "cootue-curses.ttf";
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            //ifc.GlyphExtraSpacing = { 0.0f, 0.0f };
            ifc.GlyphMaxAdvanceX = 11.0f;
            ifc.GlyphOffset = { 0.0f, 3.0f };
            m_font_map["code"] = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 14.0f, &ifc);
        }*/
        {
            ImFontConfig ifc;
            ifc.OversampleH = ifc.OversampleV = 1;
            ifc.GlyphExtraSpacing = { 1.0, 0.0 };
            m_font_map["default"] = io.Fonts->AddFontDefault(&ifc);
        }

	    // Set initial state variables here
        m_running = true;

	    return true;
    }

    bool Manager::SetupLayers()
    {
        for (auto layer : m_layer_stack)
        {
            if (!layer->Initialize())
                return false;
        }
        return true;
    }

    bool Manager::DrawGUI(float fElapsedTime)
    {
        if (!m_running)
            return false;
        if (!m_layer_stack.empty()) 
            return m_layer_stack.back()->DrawLayer(fElapsedTime);
	    return true;
    }

    void Manager::Render()
    {
        if (!m_running)
            return;

        //pge->SetDrawTarget((uint8_t)1);//Application::GUILayer());
	    ImGui::Render();
	    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    bool Manager::Shutdown()
    {
        if (!m_running)
            return false;

        while (!m_layer_stack.empty())
        {
            auto layer = m_layer_stack.back();
            m_layer_stack.pop_back();
            delete layer;
        }

	    ImGui_ImplOpenGL3_Shutdown();
        //ImGui_ImplWin32_Shutdown();
	    ImGui::DestroyContext();
        m_running = false;

	    return true;
    }

    void Manager::PushLayer(Layer* layer)
    {
	    m_layer_stack.push_back(layer);
    }

    bool Manager::PopLayer()
    {
	    if (m_layer_stack.empty())
		    return false;
        //auto layer = m_layer_stack.back();
	    m_layer_stack.pop_back();
        //delete layer;
	    return !m_layer_stack.empty();
    }

    void Manager::UpdateKeyModifiers(int mods)
    {
        if (!m_running)
            return;

        ImGuiIO& io = ImGui::GetIO();
        
        //ImGuiKeyModFlags key_mods =
        //    ((mods & (1)) ? ImGuiKey_ModCtrl : 0) |
        //    ((mods & (1 << 1)) ? ImGuiKey_ModShift : 0) |
        //    ((mods & (1 << 2)) ? ImGuiKey_ModAlt : 0) |
        //    ((mods & (1 << 3)) ? ImGuiKey_ModSuper : 0);
        //io.AddKeyEvent(key_mods);
        io.AddKeyEvent(ImGuiMod_Ctrl, IsVkDown(VK_CONTROL));
        io.AddKeyEvent(ImGuiMod_Shift, IsVkDown(VK_SHIFT));
        io.AddKeyEvent(ImGuiMod_Alt, IsVkDown(VK_MENU));
        io.AddKeyEvent(ImGuiMod_Super, IsVkDown(VK_APPS));
        // Ctrl
        //if ((mods & (1 << 0)))
        //    io.AddKeyEvent(ImGuiKey_ModCtrl, true);
        //else if (io.KeyCtrl)
        //    io.AddKeyEvent(ImGuiKey_ModCtrl, false);
        //// Shift
        //if ((mods & (1 << 1)))
        //    io.AddKeyEvent(ImGuiKey_ModShift, true);
        //else if (io.KeyShift)
        //    io.AddKeyEvent(ImGuiKey_ModShift, false);
        //// Alt
        //if ((mods & (1 << 2)))
        //    io.AddKeyEvent(ImGuiKey_ModAlt, true);
        //else if (io.KeyAlt)
        //    io.AddKeyEvent(ImGuiKey_ModAlt, false);
        //// Super
        //if ((mods & (1 << 3)))
        //    io.AddKeyEvent(ImGuiKey_ModSuper, true);
        //else if (io.KeySuper)
        //    io.AddKeyEvent(ImGuiKey_ModSuper, false);
    }

    void Manager::KeyCallback(int32_t key, bool state, int mods)
    {
        if (!m_running)
            return;

        UpdateKeyModifiers(mods);

        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey imgui_key = KeyToImGuiKey(key);
        io.AddKeyEvent(imgui_key, state);
    }

    void Manager::CursorPosCallback(HWND hwnd, int32_t x, int32_t y)
    {
        if (!m_running)
            return;

        ImGuiIO& io = ImGui::GetIO();
        /*ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
        bd->MouseHwnd = hwnd;
        if (!bd->MouseTracked)
        {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            ::TrackMouseEvent(&tme);
            bd->MouseTracked = true;
        }*/
        POINT mouse_pos = {x, y};
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            ::ClientToScreen(hwnd, &mouse_pos);
        io.AddMousePosEvent((float)mouse_pos.x, (float)mouse_pos.y);
    }

    void Manager::MouseButtonCallback(HWND hwnd, int button, bool state, int mods)
    {
        if (!m_running)
            return;

        UpdateKeyModifiers(mods);

        if (button >= 0 && button < ImGuiMouseButton_COUNT)
        {
            ImGuiIO& io = ImGui::GetIO();
            
            /*ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
            if (state)
            {
                if (bd->MouseButtonsDown == 0 && ::GetCapture() == NULL)
                    ::SetCapture(hwnd);
                bd->MouseButtonsDown |= 1 << button;
            }
            else
            {
                bd->MouseButtonsDown &= ~(1 << button);
                if (bd->MouseButtonsDown == 0 && ::GetCapture() == hwnd)
                    ::ReleaseCapture();
            } */

            io.AddMouseButtonEvent(button, state);
        }
    }

    void Manager::ScrollCallback(int32_t xoffset, int32_t yoffset)
    {
        if (!m_running)
            return;

        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(float(xoffset) / WHEEL_DELTA, (float(yoffset) / WHEEL_DELTA) * m_scroll_sensitivity);
    }

    void Manager::WindowFocusCallback(bool focused)
    {
        if (!m_running)
            return;

        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(focused);
    }

    void Manager::MouseLeaveCallback(HWND hwnd, bool state)
    {
        if (!m_running)
            return;

        ImGuiIO& io = ImGui::GetIO();
        /*ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
        if (bd->MouseHwnd == hwnd)
            bd->MouseHwnd = nullptr;
        bd->MouseTracked = false;*/
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }

    void Manager::CharCallback(unsigned int c)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(c);
    }


    //////////////////////////////////
    /// Layer class
    //////////////////////////////////
    Layer::Layer()
    {
        m_initialized = false;
        m_show_layer = true;
    }

    Layer::~Layer()
    {
        while (!m_window_list.empty())
        {
            delete m_window_list.front();
            m_window_list.pop_front();
        }

    }

    bool Layer::OnInit()
    {
	    return false;
    }

    bool Layer::OnDestroy()
    {
        return false;
    }

    bool Layer::Initialize()
    {
        if (m_initialized)
            return true;

        if (!OnInit())
            return false;

        for (auto window : m_window_list)
        {
            if (!window->Initialize())
                return false;
        }

        m_initialized = true;

        return true;
    }

    bool Layer::DrawLayer(float fElapsedTime)
    {
        if (m_show_layer)
        {
	        for (auto window : m_window_list)
	        {
		        if (!window->DrawWindow(fElapsedTime))
			        return false;
	        }
        }
	    return true;
    }

    void Layer::AddWindow(Window* window)
    {
        m_window_list.push_back(window);
    }

    void Layer::EraseWindow(Window* window)
    {
	    m_window_list.remove(window);
    }

    bool Layer::ToggleWindow(const char* name)
    {
        for (auto it = m_window_list.begin(); it != m_window_list.end(); it++)
        {
            if (!strcmp(name, (*it)->Name()))
                (*it)->ShowWindow(!(*it)->Visible());
        }
        return false;
    }

    void Layer::ShowLayer(bool show)
    {
        m_show_layer = show;
    }

    bool Layer::Visible()
    {
        return m_show_layer;
    }


    //////////////////////////////////
    /// Window class
    //////////////////////////////////
    Window::Window(const char* const title) :
        m_initialized(false),
        m_window_title(nullptr),
        m_position(nullptr),
        m_fixed_xpos(false),
        m_fixed_ypos(false),
        m_size(nullptr),
        m_fixed_width(false),
        m_fixed_height(false),
        m_window_flags(0),
        m_toggleable(true),
        m_show_window(false)
    {
        if (!title || title[0] == '\0')
            m_window_title = STR_ALLOC("Untitled Window");
        else
            m_window_title = STR_ALLOC(title);
    }

    Window::Window(const char* const title, ImVec2* position, ImVec2* size, ImGuiWindowFlags flags, bool toggleable) :
        m_initialized(false),
        m_window_title(nullptr),
        m_position(position),
        m_fixed_xpos(false),
        m_fixed_ypos(false),
        m_size(size),
        m_fixed_width(false),
        m_fixed_height(false),
        m_window_flags(flags),
        m_toggleable(toggleable),
        m_show_window(false)
    {
        if (!title || title[0] == '\0')
            m_window_title = STR_ALLOC("Untitled Window");
        else
            m_window_title = STR_ALLOC(title);
        if (m_position)
        {
            m_fixed_xpos = m_position->x > 1.0f ? true : false;
            m_fixed_ypos = m_position->y > 1.0f ? true : false;
        }
        if (m_size)
        {
            m_fixed_width = m_size->x > 1.0f ? true : false;
            m_fixed_height = m_size->y > 1.0f ? true : false;
        }
    }

    Window::~Window()
    {
    }

    bool Window::OnInit()
    {
	    return true;
    }

    bool Window::OnGUI(float fElapsedTime)
    {
	    return true;
    }

    bool Window::OnDestroy()
    {
	    return true;
    }

    bool Window::Initialize()
    {
        m_initialized = OnInit();

        return m_initialized;
    }

    void Window::SetWindowParams(float fElapsedTime)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        //ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + (viewport->Size.x / 3.0f), viewport->Pos.y + viewport->Size.y - 120.0f));
        if (m_position)
        {
            ImVec2 pos{*m_position};
            if (m_fixed_xpos)
                pos.x += LayoutPadding.x;
            else
                pos.x = pos.x * viewport->Size.x + LayoutPadding.x;
            if (m_fixed_ypos)
                pos.y += LayoutPadding.y;
            else
                pos.y = pos.y * viewport->Size.y + LayoutPadding.y;
            ImGui::SetNextWindowPos(pos);
        }
        //ImGui::SetNextWindowSize(ImVec2(viewport->Size.x / 3.0f, 120.0f));
        if (m_size)
        {
            ImVec2 size{*m_size};
            if (m_fixed_width)
                size.x -= LayoutPadding.z;
            else
                size.x = size.x * viewport->Size.x - LayoutPadding.z;
            if (m_fixed_height)
                size.y -= LayoutPadding.w;
            else
                size.y = size.y * viewport->Size.y - LayoutPadding.w;
            ImGui::SetNextWindowSize(size);
        }
        //ImGui::SetNextWindowViewport(viewport->ID);
    }

    bool Window::DrawWindow(float fElapsedTime)
    {
	    bool ret_value = true;

        if (m_show_window)
        {
            SetWindowParams(fElapsedTime);

            //ImGuiWindowFlags window_flags = 0
                //| ImGuiWindowFlags_NoDocking
                //| ImGuiWindowFlags_NoTitleBar
                //| ImGuiWindowFlags_NoResize
                //| ImGuiWindowFlags_NoMove
                //| ImGuiWindowFlags_NoScrollbar
                //| ImGuiWindowFlags_NoSavedSettings
                //;
            //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            //ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.1f, 0.1f, 0.4f, 1.0f });
            //ImGui::PushFont(Manager::GetFont("queenmab18"));
            if (m_font)
                ImGui::PushFont(m_font);
            if (ImGui::Begin(m_window_title, m_toggleable ? &m_show_window : nullptr, m_window_flags))
            {
            //ImGui::PopFont();
            //ImGui::PopStyleColor();
            //ImGui::PopStyleVar();
    	        ret_value = OnGUI(fElapsedTime);
            }
            ImGui::End();
            if (m_font)
                ImGui::PopFont();
        }

	    return ret_value;
    }

    bool Window::SetFont(const char* const key)
    {
        return (m_font = Manager::GetFont(key));
    }

    void Window::ClearFont()
    {
        m_font = nullptr;
    }

    void Window::ShowWindow(bool show)
    {
        m_show_window = show;
    }

    const char* Window::Name()
    {
        return m_window_title;
    }

    bool Window::Visible()
    {
        return m_show_window;
    }


    //////////////////////////////////
    /// Markdown class
    //////////////////////////////////
    struct Markdown : public imgui_md
    {
        virtual ImFont* get_font() const override;
    };

    ImFont* Markdown::get_font() const
    {
        if (m_is_table_header)
            return Manager::GetFont("bold");

        switch (m_hlevel)
        {
        case 0:
            if (m_is_strong)
            {
                if (m_is_em)
                    return Manager::GetFont("bold_italic");
                else
                    return Manager::GetFont("bold");
            }
            else if (m_is_em)
                return Manager::GetFont("italic");
            else
                return Manager::GetFont("regular");
        case 1:
            return Manager::GetFont("H1");
        case 2:
            return Manager::GetFont("H2");
        default:
            return Manager::GetFont("bold");
        }
    }


    //////////////////////////////////
    /// Helper
    //////////////////////////////////

    void MDText(const char* text)
    {
        static Markdown md;
        md.print(text, text + strlen(text));
    }

    static ImGuiKey KeyToImGuiKey(int32_t key)
    {
        switch (key)
        {
        case olc::Key::TAB: return ImGuiKey_Tab;
        case olc::Key::LEFT: return ImGuiKey_LeftArrow;
        case olc::Key::RIGHT: return ImGuiKey_RightArrow;
        case olc::Key::UP: return ImGuiKey_UpArrow;
        case olc::Key::DOWN: return ImGuiKey_DownArrow;
        case olc::Key::PGUP: return ImGuiKey_PageUp;
        case olc::Key::PGDN: return ImGuiKey_PageDown;
        case olc::Key::HOME: return ImGuiKey_Home;
        case olc::Key::END: return ImGuiKey_End;
        case olc::Key::INS: return ImGuiKey_Insert;
        case olc::Key::DEL: return ImGuiKey_Delete;
        case olc::Key::BACK: return ImGuiKey_Backspace;
        case olc::Key::SPACE: return ImGuiKey_Space;
        case olc::Key::ENTER: return ImGuiKey_Enter;
        case olc::Key::ESCAPE: return ImGuiKey_Escape;
        case olc::Key::OEM_7: return ImGuiKey_Apostrophe;
        case olc::Key::COMMA: return ImGuiKey_Comma;
        case olc::Key::MINUS: return ImGuiKey_Minus;
        case olc::Key::PERIOD: return ImGuiKey_Period;
        case olc::Key::OEM_2: return ImGuiKey_Slash;
        case olc::Key::OEM_1: return ImGuiKey_Semicolon;
        case olc::Key::EQUALS: return ImGuiKey_Equal;
        case olc::Key::OEM_4: return ImGuiKey_LeftBracket;
        case olc::Key::OEM_5: return ImGuiKey_Backslash;
        case olc::Key::OEM_6: return ImGuiKey_RightBracket;
        case olc::Key::OEM_3: return ImGuiKey_GraveAccent;
        case olc::Key::CAPS_LOCK: return ImGuiKey_CapsLock;
            //case olc::Key::SCROLL_LOCK: return ImGuiKey_ScrollLock;
            //case olc::Key::NUM_LOCK: return ImGuiKey_NumLock;
            //case olc::Key::PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case olc::Key::PAUSE: return ImGuiKey_Pause;
        case olc::Key::NP0: return ImGuiKey_Keypad0;
        case olc::Key::NP1: return ImGuiKey_Keypad1;
        case olc::Key::NP2: return ImGuiKey_Keypad2;
        case olc::Key::NP3: return ImGuiKey_Keypad3;
        case olc::Key::NP4: return ImGuiKey_Keypad4;
        case olc::Key::NP5: return ImGuiKey_Keypad5;
        case olc::Key::NP6: return ImGuiKey_Keypad6;
        case olc::Key::NP7: return ImGuiKey_Keypad7;
        case olc::Key::NP8: return ImGuiKey_Keypad8;
        case olc::Key::NP9: return ImGuiKey_Keypad9;
        case olc::Key::NP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case olc::Key::NP_DIV: return ImGuiKey_KeypadDivide;
        case olc::Key::NP_MUL: return ImGuiKey_KeypadMultiply;
        case olc::Key::NP_SUB: return ImGuiKey_KeypadSubtract;
        case olc::Key::NP_ADD: return ImGuiKey_KeypadAdd;
            //case olc::Key::NP_ENTER: return ImGuiKey_KeypadEnter;
            //case olc::Key::KP_EQUAL: return ImGuiKey_KeypadEqual;
        case olc::Key::SHIFT: return ImGuiKey_LeftShift;
        case olc::Key::CTRL: return ImGuiKey_LeftCtrl;
            //case olc::Key::ALT: return ImGuiKey_LeftAlt;
            //case olc::Key::LEFT_SUPER: return ImGuiKey_LeftSuper;
            //case olc::Key::RIGHT_SHIFT: return ImGuiKey_RightShift;
            //case olc::Key::RIGHT_CONTROL: return ImGuiKey_RightCtrl;
            //case olc::Key::RIGHT_ALT: return ImGuiKey_RightAlt;
            //case olc::Key::RIGHT_SUPER: return ImGuiKey_RightSuper;
            //case olc::Key::MENU: return ImGuiKey_Menu;
        case olc::Key::K0: return ImGuiKey_0;
        case olc::Key::K1: return ImGuiKey_1;
        case olc::Key::K2: return ImGuiKey_2;
        case olc::Key::K3: return ImGuiKey_3;
        case olc::Key::K4: return ImGuiKey_4;
        case olc::Key::K5: return ImGuiKey_5;
        case olc::Key::K6: return ImGuiKey_6;
        case olc::Key::K7: return ImGuiKey_7;
        case olc::Key::K8: return ImGuiKey_8;
        case olc::Key::K9: return ImGuiKey_9;
        case olc::Key::A: return ImGuiKey_A;
        case olc::Key::B: return ImGuiKey_B;
        case olc::Key::C: return ImGuiKey_C;
        case olc::Key::D: return ImGuiKey_D;
        case olc::Key::E: return ImGuiKey_E;
        case olc::Key::F: return ImGuiKey_F;
        case olc::Key::G: return ImGuiKey_G;
        case olc::Key::H: return ImGuiKey_H;
        case olc::Key::I: return ImGuiKey_I;
        case olc::Key::J: return ImGuiKey_J;
        case olc::Key::K: return ImGuiKey_K;
        case olc::Key::L: return ImGuiKey_L;
        case olc::Key::M: return ImGuiKey_M;
        case olc::Key::N: return ImGuiKey_N;
        case olc::Key::O: return ImGuiKey_O;
        case olc::Key::P: return ImGuiKey_P;
        case olc::Key::Q: return ImGuiKey_Q;
        case olc::Key::R: return ImGuiKey_R;
        case olc::Key::S: return ImGuiKey_S;
        case olc::Key::T: return ImGuiKey_T;
        case olc::Key::U: return ImGuiKey_U;
        case olc::Key::V: return ImGuiKey_V;
        case olc::Key::W: return ImGuiKey_W;
        case olc::Key::X: return ImGuiKey_X;
        case olc::Key::Y: return ImGuiKey_Y;
        case olc::Key::Z: return ImGuiKey_Z;
        case olc::Key::F1: return ImGuiKey_F1;
        case olc::Key::F2: return ImGuiKey_F2;
        case olc::Key::F3: return ImGuiKey_F3;
        case olc::Key::F4: return ImGuiKey_F4;
        case olc::Key::F5: return ImGuiKey_F5;
        case olc::Key::F6: return ImGuiKey_F6;
        case olc::Key::F7: return ImGuiKey_F7;
        case olc::Key::F8: return ImGuiKey_F8;
        case olc::Key::F9: return ImGuiKey_F9;
        case olc::Key::F10: return ImGuiKey_F10;
        case olc::Key::F11: return ImGuiKey_F11;
        case olc::Key::F12: return ImGuiKey_F12;
        case olc::Key::NONE:
        default: return ImGuiKey_None;
        }
    }

    unsigned int KeyToChar(int32_t key, bool shift, bool capslock)
    {
        if (capslock && key != olc::Key::ENTER)
            shift = !shift;
        switch (key)
        {
        case olc::Key::TAB: return '\t';
        //case olc::Key::LEFT: return ImGuiKey_LeftArrow;
        //case olc::Key::RIGHT: return ImGuiKey_RightArrow;
        //case olc::Key::UP: return ImGuiKey_UpArrow;
        //case olc::Key::DOWN: return ImGuiKey_DownArrow;
        //case olc::Key::PGUP: return ImGuiKey_PageUp;
        //case olc::Key::PGDN: return ImGuiKey_PageDown;
        //case olc::Key::HOME: return ImGuiKey_Home;
        //case olc::Key::END: return ImGuiKey_End;
        //case olc::Key::INS: return ImGuiKey_Insert;
        //case olc::Key::DEL: return ImGuiKey_Delete;
        //case olc::Key::BACK: return '\b';
        case olc::Key::SPACE: return ' ';
        case olc::Key::ENTER: return shift ? '\n' : '\r';
        case olc::Key::ESCAPE: return 0x1B;                     // Might be Windows only?
        case olc::Key::OEM_7: return shift ? '\"' : '\'';
        case olc::Key::COMMA: return shift ? '<' : ',';
        case olc::Key::MINUS: return shift ? '_' : '-';
        case olc::Key::PERIOD: return shift ? '>' : '.';
        case olc::Key::OEM_2: return shift ? '?' : '/';
        case olc::Key::OEM_1: return shift ? ':' : ';';
        case olc::Key::EQUALS: return shift ? '+' : '=';
        case olc::Key::OEM_4: return shift ? '{' : '[';
        case olc::Key::OEM_5: return shift ? '|' : '\\';
        case olc::Key::OEM_6: return shift ? '}' : ']';
        case olc::Key::OEM_3: return shift ? '~' : '`';
        //case olc::Key::CAPS_LOCK: return ImGuiKey_CapsLock;
        //case olc::Key::SCROLL_LOCK: return ImGuiKey_ScrollLock;
        //case olc::Key::NUM_LOCK: return ImGuiKey_NumLock;
        //case olc::Key::PRINT_SCREEN: return ImGuiKey_PrintScreen;
        //case olc::Key::PAUSE: return ImGuiKey_Pause;
        case olc::Key::NP0: return '0';
        case olc::Key::NP1: return '1';
        case olc::Key::NP2: return '2';
        case olc::Key::NP3: return '3';
        case olc::Key::NP4: return '4';
        case olc::Key::NP5: return '5';
        case olc::Key::NP6: return '6';
        case olc::Key::NP7: return '7';
        case olc::Key::NP8: return '8';
        case olc::Key::NP9: return '9';
        case olc::Key::NP_DECIMAL: return '.';
        case olc::Key::NP_DIV: return '/';
        case olc::Key::NP_MUL: return '*';
        case olc::Key::NP_SUB: return '-';
        case olc::Key::NP_ADD: return '+';
        //case olc::Key::NP_ENTER: return ImGuiKey_KeypadEnter;
        //case olc::Key::KP_EQUAL: return ImGuiKey_KeypadEqual;
        //case olc::Key::SHIFT: return ImGuiKey_LeftShift;
        //case olc::Key::CTRL: return ImGuiKey_LeftCtrl;
        //case olc::Key::ALT: return ImGuiKey_LeftAlt;
        //case olc::Key::LEFT_SUPER: return ImGuiKey_LeftSuper;
        //case olc::Key::RIGHT_SHIFT: return ImGuiKey_RightShift;
        //case olc::Key::RIGHT_CONTROL: return ImGuiKey_RightCtrl;
        //case olc::Key::RIGHT_ALT: return ImGuiKey_RightAlt;
        //case olc::Key::RIGHT_SUPER: return ImGuiKey_RightSuper;
        //case olc::Key::MENU: return ImGuiKey_Menu;
        case olc::Key::K0: return shift ? ')' : '0';
        case olc::Key::K1: return shift ? '!' : '1';
        case olc::Key::K2: return shift ? '@' : '2';
        case olc::Key::K3: return shift ? '#' : '3';
        case olc::Key::K4: return shift ? '$' : '4';
        case olc::Key::K5: return shift ? '%' : '5';
        case olc::Key::K6: return shift ? '^' : '6';
        case olc::Key::K7: return shift ? '&' : '7';
        case olc::Key::K8: return shift ? '*' : '8';
        case olc::Key::K9: return shift ? '(' : '9';
        case olc::Key::A: return shift ? 'A' : 'a';
        case olc::Key::B: return shift ? 'B' : 'b';
        case olc::Key::C: return shift ? 'C' : 'c';
        case olc::Key::D: return shift ? 'D' : 'd';
        case olc::Key::E: return shift ? 'E' : 'e';
        case olc::Key::F: return shift ? 'F' : 'f';
        case olc::Key::G: return shift ? 'G' : 'g';
        case olc::Key::H: return shift ? 'H' : 'h';
        case olc::Key::I: return shift ? 'I' : 'i';
        case olc::Key::J: return shift ? 'J' : 'j';
        case olc::Key::K: return shift ? 'K' : 'k';
        case olc::Key::L: return shift ? 'L' : 'l';
        case olc::Key::M: return shift ? 'M' : 'm';
        case olc::Key::N: return shift ? 'N' : 'n';
        case olc::Key::O: return shift ? 'O' : 'o';
        case olc::Key::P: return shift ? 'P' : 'p';
        case olc::Key::Q: return shift ? 'Q' : 'q';
        case olc::Key::R: return shift ? 'R' : 'r';
        case olc::Key::S: return shift ? 'S' : 's';
        case olc::Key::T: return shift ? 'T' : 't';
        case olc::Key::U: return shift ? 'U' : 'u';
        case olc::Key::V: return shift ? 'V' : 'v';
        case olc::Key::W: return shift ? 'W' : 'w';
        case olc::Key::X: return shift ? 'X' : 'x';
        case olc::Key::Y: return shift ? 'Y' : 'y';
        case olc::Key::Z: return shift ? 'Z' : 'z';
        //case olc::Key::F1: return ImGuiKey_F1;
        //case olc::Key::F2: return ImGuiKey_F2;
        //case olc::Key::F3: return ImGuiKey_F3;
        //case olc::Key::F4: return ImGuiKey_F4;
        //case olc::Key::F5: return ImGuiKey_F5;
        //case olc::Key::F6: return ImGuiKey_F6;
        //case olc::Key::F7: return ImGuiKey_F7;
        //case olc::Key::F8: return ImGuiKey_F8;
        //case olc::Key::F9: return ImGuiKey_F9;
        //case olc::Key::F10: return ImGuiKey_F10;
        //case olc::Key::F11: return ImGuiKey_F11;
        //case olc::Key::F12: return ImGuiKey_F12;
        case olc::Key::NONE:
        default: return '\0';
        }

    }

    static void StyleColors()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.28f, 0.14f, 0.14f, 0.686f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
        colors[ImGuiCol_Border] = ImVec4(0.97f, 0.00f, 0.00f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.69f, 0.53f, 0.47f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.68f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.44f, 0.23f, 0.23f, 0.75f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.67f, 0.00f, 0.00f, 0.87f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.80f, 0.40f, 0.40f, 0.20f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.30f, 0.25f, 0.20f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.56f, 0.40f, 0.30f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.80f, 0.55f, 0.40f, 0.40f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.46f, 0.39f, 0.60f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
        colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
        colors[ImGuiCol_Button] = ImVec4(0.66f, 0.40f, 0.32f, 0.69f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.71f, 0.43f, 0.31f, 0.79f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.68f, 0.35f, 0.27f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.90f, 0.62f, 0.40f, 0.45f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.90f, 0.63f, 0.45f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.87f, 0.71f, 0.53f, 0.80f);
        colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
        colors[ImGuiCol_Tab] = ImVec4(0.90f, 0.52f, 0.40f, 0.45f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.90f, 0.45f, 0.45f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.63f, 0.16f, 0.10f, 0.80f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
        colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


    }

    float SetNextItemWidth(float percent, float min, float max)
    {
        //(0.25f * content_size.x) - (content_size.x - ImGui::GetContentRegionAvail().x)
        float value = std::clamp((percent * max) - (max - ImGui::GetContentRegionAvail().x), min, max);
        ImGui::SetNextItemWidth(value);
        return value;
    }

    bool IsShortcutPressed(ImGuiKey mods, ImGuiKey key/*, ImGuiID owner_id = 0*/)
    {
        if (ImGui::GetIO().KeyMods != mods)
            return false;
        if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
            return false;
        //if (owner_id == 0)
        //    owner_id = ImGui::GetID("");
        //if (!ImGui::TestInputOwner(key, owner_id))
        //    return false;
        //ImGui::SetInputOwner(key, owner_id);
        return ImGui::IsKeyPressed(key, false);// , owner_id);
    };
}
