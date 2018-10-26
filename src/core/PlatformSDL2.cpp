#include "../core/Base.h"
#include "../core/Platform.h"
#include "../core/Game.h"


#include <SDL2/SDL_config.h>
#include <SDL2/SDL.h>
    #ifdef GP_PLATFORM_LINUX
#define SDL_VIDEO_DRIVER_X11
#endif
#include <SDL2/SDL_syswm.h>
#include <bgfx/platform.h>

#include "../renderer/Renderer.h"
#include "../renderer/BGFXImGui.h"

int __app_argc = 0;
char** __app_argv = nullptr;


namespace gplay {

// sdl window
static SDL_Window * __window;
static int __windowSize[2];

// timer
static double __timeAbsolute;
static std::chrono::time_point<std::chrono::high_resolution_clock> __timeStart;

// mouse input
static bool __mouseCaptured = false;
static float __mouseCapturePointX = 0;
static float __mouseCapturePointY = 0;
static bool __cursorVisible = true;

// keys
uint16_t _translateKey[256];

// mobile device gesture
static bool __multiTouch = false;


extern int strcmpnocase(const char* s1, const char* s2)
{
#ifdef GP_PLATFORM_LINUX
    return strcasecmp(s1, s2);
#elif GP_PLATFORM_WINDOWS
    return _strcmpi(s1, s2);
#elif GP_PLATFORM_MACOS
    return strcasecmp(s1, s2);
#elif GP_PLATFORM_ANDROID
    return strcasecmp(s1, s2);
#elif GP_PLATFORM_IOS
    return strcasecmp(s1, s2);
#else
    GP_ERROR("Fix me !");
    return 0;
#endif
}

extern void print(const char* format, ...)
{
#if defined (GP_PLATFORM_WINDOWS)
    va_list argptr;
    va_start(argptr, format);
    int sz = vfprintf(stderr, format, argptr);
    if (sz > 0)
    {
        char* buf = new char[sz + 1];
        vsprintf(buf, format, argptr);
        buf[sz] = 0;
        OutputDebugStringA(buf);
        SAFE_DELETE_ARRAY(buf);
    }
    va_end(argptr);
#elif defined (GP_PLATFORM_LINUX) || defined (GP_PLATFORM_MACOS)
    GP_ASSERT(format);
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
#endif
}

void updateWindowSize()
{
    GP_ASSERT(__window);

    int width;
    int height;
    SDL_GetWindowSize(__window, &width, &height);

    Renderer::getInstance().resize(width, height);

    __windowSize[0] = width;
    __windowSize[1] = height;
}

inline bool setWindowForBgfx(SDL_Window* _window)
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(_window, &wmi) )
    {
        return false;
    }

    bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt          = wmi.info.x11.display;
    pd.nwh          = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
    pd.ndt          = NULL;
    pd.nwh          = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    pd.ndt          = NULL;
    pd.nwh          = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
    pd.ndt          = wmi.info.vivante.display;
    pd.nwh          = wmi.info.vivante.window;
#endif // BX_PLATFORM_
    pd.context      = NULL;
    pd.backBuffer   = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);

    return true;
}

void initTranslateKey(uint16_t sdl, Keyboard::Key key)
{
    _translateKey[sdl & 0xffff] = (uint16_t)key;
}

Keyboard::Key translateKey(SDL_Scancode sdl)
{
    return (Keyboard::Key)_translateKey[sdl & 0xffff];
}



//-------------------------------------------------------------------------------------------------------------
// ImGui-SDL2 binding
//-------------------------------------------------------------------------------------------------------------

static bool g_MousePressed[3] = { false, false, false };
static SDL_Cursor* g_MouseCursors[ImGuiMouseCursor_Count_] = { 0 };
static Uint64 g_Time = 0;


static const char* ImGui_ImplSdlGL3_GetClipboardText(void*)
{
    return SDL_GetClipboardText();
}

static void ImGui_ImplSdlGL3_SetClipboardText(void*, const char* text)
{
    SDL_SetClipboardText(text);
}

static void ImGui_ImplSdlGL3_Shutdown()
{
    // Destroy SDL mouse cursors
    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_Count_; cursor_n++)
        SDL_FreeCursor(g_MouseCursors[cursor_n]);
    memset(g_MouseCursors, 0, sizeof(g_MouseCursors));

    // Destroy bgfx imgui objects
    GPImGui::Get()->imguiShutdown();
}

static bool ImGui_ImplSdlGL3_Init(SDL_Window* window)
{
    ImGui::StyleColorsClassic();

    // Create bgfx imgui objects
    GPImGui::Get()->imguiInit();

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;       // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;        // We can honor io.WantSetMousePos requests (optional, rarely used)

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

    io.SetClipboardTextFn = ImGui_ImplSdlGL3_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplSdlGL3_GetClipboardText;
    io.ClipboardUserData = NULL;

    g_MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    g_MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    g_MouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#else
    (void)window;
#endif

    return true;
}

static void ImGui_ImplSDL2_UpdateMousePosAndButtons()
{
    ImGuiIO& io = ImGui::GetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
        SDL_WarpMouseInWindow(__window, (int)io.MousePos.x, (int)io.MousePos.y);
    else
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    int mx, my;
    Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
    io.MouseDown[0] = g_MousePressed[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = g_MousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = g_MousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;

#if SDL_HAS_CAPTURE_MOUSE && !defined(__EMSCRIPTEN__)
    SDL_Window* focused_window = SDL_GetKeyboardFocus();
    if (g_Window == focused_window)
    {
        // SDL_GetMouseState() gives mouse position seemingly based on the last window entered/focused(?)
        // The creation of a new windows at runtime and SDL_CaptureMouse both seems to severely mess up with that, so we retrieve that position globally.
        int wx, wy;
        SDL_GetWindowPosition(focused_window, &wx, &wy);
        SDL_GetGlobalMouseState(&mx, &my);
        mx -= wx;
        my -= wy;
        io.MousePos = ImVec2((float)mx, (float)my);
    }

    // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger the OS window resize cursor.
    // The function is only supported from SDL 2.0.4 (released Jan 2016)
    bool any_mouse_button_down = ImGui::IsAnyMouseDown();
    SDL_CaptureMouse(any_mouse_button_down ? SDL_TRUE : SDL_FALSE);
#else
    if (SDL_GetWindowFlags(__window) & SDL_WINDOW_INPUT_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);
#endif
}

static void ImGui_ImplSDL2_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        SDL_ShowCursor(SDL_FALSE);
    }
    else
    {
        // Show OS mouse cursor
        SDL_SetCursor(g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
        SDL_ShowCursor(SDL_TRUE);
    }
}

static void ImGui_ImplSdlGL3_NewFrame(SDL_Window* window)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt());     // Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame()

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
    static Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 current_time = SDL_GetPerformanceCounter();
    io.DeltaTime = g_Time > 0 ? (float)((double)(current_time - g_Time) / frequency) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    ImGui_ImplSDL2_UpdateMousePosAndButtons();
    ImGui_ImplSDL2_UpdateMouseCursor();

    // Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
    ImGui::NewFrame();

    // fix issue when debugging (mouse was still captured by sdl and we cannot use it in debugger)
    //SDL_CaptureMouse(SDL_FALSE);
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
static bool ImGui_ImplSdlGL3_ProcessEvent(SDL_Event* event)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type)
    {
    case SDL_MOUSEWHEEL:
        {
            if (event->wheel.x > 0) io.MouseWheelH += 1;
            if (event->wheel.x < 0) io.MouseWheelH -= 1;
            if (event->wheel.y > 0) io.MouseWheel += 1;
            if (event->wheel.y < 0) io.MouseWheel -= 1;
            return true;
        }
    case SDL_MOUSEBUTTONDOWN:
        {
            if (event->button.button == SDL_BUTTON_LEFT) g_MousePressed[0] = true;
            if (event->button.button == SDL_BUTTON_RIGHT) g_MousePressed[1] = true;
            if (event->button.button == SDL_BUTTON_MIDDLE) g_MousePressed[2] = true;
            return true;
        }
    case SDL_TEXTINPUT:
        {
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        {
            int key = event->key.keysym.scancode;
            IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
            io.KeysDown[key] = (event->type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
            io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
            return true;
        }
    }
    return false;
}




//-------------------------------------------------------------------------------------------------------------
// Platform SDL2 impl
//-------------------------------------------------------------------------------------------------------------

Platform::Platform(Game* game) :
    _game(game)
{
    GP_ASSERT(__app_argc);
    GP_ASSERT(__app_argv);

    std::memset(_translateKey, 0, sizeof(_translateKey));
    initTranslateKey(SDL_SCANCODE_ESCAPE,       Keyboard::KEY_ESCAPE);
    initTranslateKey(SDL_SCANCODE_RETURN,       Keyboard::KEY_RETURN);
    initTranslateKey(SDL_SCANCODE_TAB,          Keyboard::KEY_TAB);
    initTranslateKey(SDL_SCANCODE_BACKSPACE,    Keyboard::KEY_BACKSPACE);
    initTranslateKey(SDL_SCANCODE_SPACE,        Keyboard::KEY_SPACE);
    initTranslateKey(SDL_SCANCODE_UP,           Keyboard::KEY_UP_ARROW);
    initTranslateKey(SDL_SCANCODE_DOWN,         Keyboard::KEY_DOWN_ARROW);
    initTranslateKey(SDL_SCANCODE_LEFT,         Keyboard::KEY_LEFT_ARROW);
    initTranslateKey(SDL_SCANCODE_RIGHT,        Keyboard::KEY_RIGHT_ARROW);
    initTranslateKey(SDL_SCANCODE_PAGEUP,       Keyboard::KEY_PG_UP);
    initTranslateKey(SDL_SCANCODE_PAGEDOWN,     Keyboard::KEY_PG_DOWN);
    initTranslateKey(SDL_SCANCODE_HOME,         Keyboard::KEY_HOME);
    initTranslateKey(SDL_SCANCODE_END,          Keyboard::KEY_END);
    initTranslateKey(SDL_SCANCODE_PRINTSCREEN,  Keyboard::KEY_PRINT);
    initTranslateKey(SDL_SCANCODE_KP_PLUS,      Keyboard::KEY_PLUS);
    initTranslateKey(SDL_SCANCODE_EQUALS,       Keyboard::KEY_PLUS);
    initTranslateKey(SDL_SCANCODE_KP_MINUS,     Keyboard::KEY_MINUS);
    initTranslateKey(SDL_SCANCODE_MINUS,        Keyboard::KEY_MINUS);
    initTranslateKey(SDL_SCANCODE_GRAVE,        Keyboard::KEY_TILDE);
    initTranslateKey(SDL_SCANCODE_KP_COMMA,     Keyboard::KEY_COMMA);
    initTranslateKey(SDL_SCANCODE_COMMA,        Keyboard::KEY_COMMA);
    initTranslateKey(SDL_SCANCODE_KP_PERIOD,    Keyboard::KEY_PERIOD);
    initTranslateKey(SDL_SCANCODE_PERIOD,       Keyboard::KEY_PERIOD);
    initTranslateKey(SDL_SCANCODE_SLASH,        Keyboard::KEY_SLASH);
    initTranslateKey(SDL_SCANCODE_F1,           Keyboard::KEY_F1);
    initTranslateKey(SDL_SCANCODE_F2,           Keyboard::KEY_F2);
    initTranslateKey(SDL_SCANCODE_F3,           Keyboard::KEY_F3);
    initTranslateKey(SDL_SCANCODE_F4,           Keyboard::KEY_F4);
    initTranslateKey(SDL_SCANCODE_F5,           Keyboard::KEY_F5);
    initTranslateKey(SDL_SCANCODE_F6,           Keyboard::KEY_F6);
    initTranslateKey(SDL_SCANCODE_F7,           Keyboard::KEY_F7);
    initTranslateKey(SDL_SCANCODE_F8,           Keyboard::KEY_F8);
    initTranslateKey(SDL_SCANCODE_F9,           Keyboard::KEY_F9);
    initTranslateKey(SDL_SCANCODE_F10,          Keyboard::KEY_F10);
    initTranslateKey(SDL_SCANCODE_F11,          Keyboard::KEY_F11);
    initTranslateKey(SDL_SCANCODE_F12,          Keyboard::KEY_F12);
    initTranslateKey(SDL_SCANCODE_KP_0,         Keyboard::KEY_KP_INSERT);
    initTranslateKey(SDL_SCANCODE_KP_1,         Keyboard::KEY_KP_END);
    initTranslateKey(SDL_SCANCODE_KP_2,         Keyboard::KEY_KP_DOWN);
    initTranslateKey(SDL_SCANCODE_KP_3,         Keyboard::KEY_KP_PG_DOWN);
    initTranslateKey(SDL_SCANCODE_KP_4,         Keyboard::KEY_KP_LEFT);
    initTranslateKey(SDL_SCANCODE_KP_5,         Keyboard::KEY_KP_FIVE);
    initTranslateKey(SDL_SCANCODE_KP_6,         Keyboard::KEY_KP_RIGHT);
    initTranslateKey(SDL_SCANCODE_KP_7,         Keyboard::KEY_KP_HOME);
    initTranslateKey(SDL_SCANCODE_KP_8,         Keyboard::KEY_KP_UP);
    initTranslateKey(SDL_SCANCODE_KP_9,         Keyboard::KEY_PG_UP);
    initTranslateKey(SDL_SCANCODE_0,            Keyboard::KEY_ZERO);
    initTranslateKey(SDL_SCANCODE_1,            Keyboard::KEY_ONE);
    initTranslateKey(SDL_SCANCODE_2,            Keyboard::KEY_TWO);
    initTranslateKey(SDL_SCANCODE_3,            Keyboard::KEY_THREE);
    initTranslateKey(SDL_SCANCODE_4,            Keyboard::KEY_FOUR);
    initTranslateKey(SDL_SCANCODE_5,            Keyboard::KEY_FIVE);
    initTranslateKey(SDL_SCANCODE_6,            Keyboard::KEY_SIX);
    initTranslateKey(SDL_SCANCODE_7,            Keyboard::KEY_SEVEN);
    initTranslateKey(SDL_SCANCODE_8,            Keyboard::KEY_EIGHT);
    initTranslateKey(SDL_SCANCODE_9,            Keyboard::KEY_NINE);
    initTranslateKey(SDL_SCANCODE_A,            Keyboard::KEY_A);
    initTranslateKey(SDL_SCANCODE_B,            Keyboard::KEY_B);
    initTranslateKey(SDL_SCANCODE_C,            Keyboard::KEY_C);
    initTranslateKey(SDL_SCANCODE_D,            Keyboard::KEY_D);
    initTranslateKey(SDL_SCANCODE_E,            Keyboard::KEY_E);
    initTranslateKey(SDL_SCANCODE_F,            Keyboard::KEY_F);
    initTranslateKey(SDL_SCANCODE_G,            Keyboard::KEY_G);
    initTranslateKey(SDL_SCANCODE_H,            Keyboard::KEY_H);
    initTranslateKey(SDL_SCANCODE_I,            Keyboard::KEY_I);
    initTranslateKey(SDL_SCANCODE_J,            Keyboard::KEY_J);
    initTranslateKey(SDL_SCANCODE_K,            Keyboard::KEY_K);
    initTranslateKey(SDL_SCANCODE_L,            Keyboard::KEY_L);
    initTranslateKey(SDL_SCANCODE_M,            Keyboard::KEY_M);
    initTranslateKey(SDL_SCANCODE_N,            Keyboard::KEY_N);
    initTranslateKey(SDL_SCANCODE_O,            Keyboard::KEY_O);
    initTranslateKey(SDL_SCANCODE_P,            Keyboard::KEY_P);
    initTranslateKey(SDL_SCANCODE_Q,            Keyboard::KEY_Q);
    initTranslateKey(SDL_SCANCODE_R,            Keyboard::KEY_R);
    initTranslateKey(SDL_SCANCODE_S,            Keyboard::KEY_S);
    initTranslateKey(SDL_SCANCODE_T,            Keyboard::KEY_T);
    initTranslateKey(SDL_SCANCODE_U,            Keyboard::KEY_U);
    initTranslateKey(SDL_SCANCODE_V,            Keyboard::KEY_V);
    initTranslateKey(SDL_SCANCODE_W,            Keyboard::KEY_W);
    initTranslateKey(SDL_SCANCODE_X,            Keyboard::KEY_X);
    initTranslateKey(SDL_SCANCODE_Y,            Keyboard::KEY_Y);
    initTranslateKey(SDL_SCANCODE_Z,            Keyboard::KEY_Z);
}

Platform::~Platform()
{
}

Platform* Platform::create(Game* game, void* externalWindow)
{
    GP_ASSERT(game);

    FileSystem::setResourcePath("./");
    Platform* platform = new Platform(game);

    // Get the window configuration values
    const char *title = NULL;
    int __x = 0, __y = 0, __width = 1280, __height = 800, __samples = 0;
    bool fullscreen = false;
    if (game->getConfig())
    {
        Properties* config = game->getConfig()->getNamespace("window", true);
        if (config)
        {
            // Read window title.
            title = config->getString("title");

            // Read window rect.
            int x = config->getInt("x");
            int y = config->getInt("y");
            int width = config->getInt("width");
            int height = config->getInt("height");
            int samples = config->getInt("samples");
            fullscreen = config->getBool("fullscreen");

            if (fullscreen && width == 0 && height == 0)
            {
                // Use the screen resolution if fullscreen is true but width and height were not set in the config
                SDL_DisplayMode displayMode;
                SDL_GetCurrentDisplayMode(0, &displayMode);
                width = displayMode.w;
                height = displayMode.h;

            }
            if (x != 0) __x = x;
            if (y != 0) __y = y;
            if (width != 0) __width = width;
            if (height != 0) __height = height;
            if (samples != 0) __samples = samples;
        }
    }

    // Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        print("Failed to init SDL\n");
        return nullptr;
    }

    if(externalWindow)
    {
        __window = SDL_CreateWindowFrom(externalWindow);
        SDL_GetWindowSize(__window, &__width, &__height);
    }
    else
    {
        __window = SDL_CreateWindow(title,
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      __width,
                                      __height,
                                      SDL_WINDOW_SHOWN);

    }

    // Check that everything worked out okay
    if (!__window)
    {
        print("Unable to create window\n");
        return nullptr;
    }


    // init bgfx

    bgfx::RendererType::Enum supportedTypes[bgfx::RendererType::Count];
    uint8_t count =  bgfx::getSupportedRenderers(bgfx::RendererType::Count, supportedTypes);
    for(uint8_t i=0; i<count; i++)
        print("supported type [%d] = %s\n", i, bgfx::getRendererName(supportedTypes[i]));


    setWindowForBgfx(__window);

    // Init bgfx
    bgfx::Init init;
    init.type = bgfx::RendererType::OpenGL;
    bgfx::init(init);

    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    bgfx::reset(__width, __height, reset);
    bgfx::setDebug(debug);

    BGFXRenderer::initInstance();
    Renderer::getInstance().queryCaps();

    game->setVsync(true);
    game->setViewport(Rectangle(0,0,__width,__height));

    updateWindowSize();



    // Create ImGui context and init
    ImGui::CreateContext();
    ImGui_ImplSdlGL3_Init(__window);

    return platform;
}

void Platform::start()
{
    GP_ASSERT(_game);

    // Get the initial time.
    __timeStart = std::chrono::high_resolution_clock::now();
    __timeAbsolute = 0L;

    // Run the game.
    _game->run();
}

void Platform::stop()
{
    // shutdow imgui
    ImGui_ImplSdlGL3_Shutdown();

    // shutdow bgfx
    bgfx::shutdown();

    // shutdow sdl
    SDL_DestroyWindow(__window);
    SDL_Quit();
}

void Platform::frame()
{
    if (_game)
    {
        Renderer::getInstance().beginFrame();

        ImGui_ImplSdlGL3_NewFrame(__window);

        _game->frame();

        ImGui::Render();
        GPImGui::Get()->imguiRender(ImGui::GetDrawData());

        Renderer::getInstance().endFrame();
    }
}

int Platform::processEvents()
{
    SDL_Event evt;

    while (SDL_PollEvent(&evt))
    {
        // Process ImGui events
        ImGui_ImplSdlGL3_ProcessEvent(&evt);

        // Process SDL2 events
        switch (evt.type)
        {
            case SDL_QUIT:
            {
                _game->exit();
                return 0;
            }
            break;

            case SDL_MOUSEWHEEL:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                const SDL_MouseWheelEvent& wheelEvent = evt.wheel;
                int wheelDelta = wheelEvent.y;
                gplay::Platform::mouseEventInternal(gplay::Mouse::MOUSE_WHEEL, 0, 0, wheelDelta);
            }
            break;

            case SDL_MOUSEBUTTONDOWN:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                gplay::Mouse::MouseEvent mouseEvt;
                const SDL_MouseButtonEvent& sdlMouseEvent = evt.button;

                switch (sdlMouseEvent.button)
                {
                case SDL_BUTTON_LEFT:
                    mouseEvt = gplay::Mouse::MOUSE_PRESS_LEFT_BUTTON;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseEvt = gplay::Mouse::MOUSE_PRESS_RIGHT_BUTTON;
                    break;
                case SDL_BUTTON_MIDDLE:
                    mouseEvt = gplay::Mouse::MOUSE_PRESS_MIDDLE_BUTTON;
                    break;
                }

                if (!gplay::Platform::mouseEventInternal(mouseEvt, sdlMouseEvent.x, sdlMouseEvent.y, 0))
                {
                    gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_PRESS, sdlMouseEvent.x, sdlMouseEvent.y, 0, true);
                }
            }
            break;

            case SDL_MOUSEBUTTONUP:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                gplay::Mouse::MouseEvent mouseEvt;
                const SDL_MouseButtonEvent& sdlMouseEvent = evt.button;

                switch (sdlMouseEvent.button)
                {
                case SDL_BUTTON_LEFT:
                    mouseEvt = gplay::Mouse::MOUSE_RELEASE_LEFT_BUTTON;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseEvt = gplay::Mouse::MOUSE_RELEASE_RIGHT_BUTTON;
                    break;
                case SDL_BUTTON_MIDDLE:
                    mouseEvt = gplay::Mouse::MOUSE_RELEASE_MIDDLE_BUTTON;
                    break;
                }

                if (!gplay::Platform::mouseEventInternal(mouseEvt, sdlMouseEvent.x, sdlMouseEvent.y, 0))
                {
                    gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_RELEASE, sdlMouseEvent.x, sdlMouseEvent.y, 0, true);
                }
            }
            break;

            case SDL_MOUSEMOTION:
            {
                if(ImGui::GetIO().WantCaptureMouse)
                    continue;

                const SDL_MouseMotionEvent& motionEvt = evt.motion;

                int x = motionEvt.x;
                int y = motionEvt.y;
                if (__mouseCaptured)
                {
                    if (x == __mouseCapturePointX && y == __mouseCapturePointY)
                    {
                        // Discard the first MotionNotify following capture since it contains bogus x,y data.
                        break;
                    }

                    // Convert to deltas
                    x -= __mouseCapturePointX;
                    y -= __mouseCapturePointY;

                    // Warp mouse back to center of screen.
                    SDL_WarpMouseInWindow(__window, __mouseCapturePointX, __mouseCapturePointY);
                }

                if (!gplay::Platform::mouseEventInternal(gplay::Mouse::MOUSE_MOVE, x, y, 0))
                {
                    //if (evt.xmotion.state & Button1Mask)
                    if (evt.button.button == SDL_BUTTON_LEFT)
                    {
                        gplay::Platform::touchEventInternal(gplay::Touch::TOUCH_MOVE, x, y, 0, true);
                    }
                }
            }
            break;


            case SDL_KEYDOWN:
            {
                if(!ImGui::GetIO().WantTextInput)
                {
                    const SDL_KeyboardEvent& keyEvent = evt.key;
                    Keyboard::Key key = translateKey(keyEvent.keysym.scancode);
                    gplay::Platform::keyEventInternal(gplay::Keyboard::KEY_PRESS, key);

                    switch (key)
                    {
                    case Keyboard::KEY_F1:
                        Renderer::getInstance().toggleDebugStats();
                        break;
                    case Keyboard::KEY_F3:
                        Renderer::getInstance().toggleWireFrame();
                        break;
                    case Keyboard::KEY_F7:
                        Renderer::getInstance().toggleVSync();
                        break;
                    }
                }
            }
            break;

            case SDL_KEYUP:
            {
                if(!ImGui::GetIO().WantTextInput)
                {
                    const SDL_KeyboardEvent& keyEvent = evt.key;
                    Keyboard::Key key = translateKey(keyEvent.keysym.scancode);
                    gplay::Platform::keyEventInternal(gplay::Keyboard::KEY_RELEASE, key);
                }
            }
            break;
        }
    }

    return 1;
}

void Platform::swapBuffers()
{
    Renderer::getInstance().endFrame();
}

void Platform::signalShutdown()
{
    GP_ERROR("Fix me !");
}

bool Platform::canExit()
{
#if defined (GP_PLATFORM_IOS)
    return false;
#else
    return true;
#endif

    GP_ERROR("Fix me !");
    return false;
}

unsigned int Platform::getDisplayWidth()
{
    return __windowSize[0];
}

unsigned int Platform::getDisplayHeight()
{
    return __windowSize[1];
}

double Platform::getAbsoluteTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    typedef std::chrono::duration<double, std::milli> duration;
    duration elapsed = now - __timeStart;
    __timeAbsolute = elapsed.count();
    return __timeAbsolute;
}

void Platform::setAbsoluteTime(double time)
{
    __timeAbsolute = time;
}

bool Platform::isVsync()
{
    return Renderer::getInstance().isVSync();
}

void Platform::setVsync(bool enable)
{
     Renderer::getInstance().setVSync(enable);
}

void Platform::sleep(long ms)
{
    SDL_Delay(ms);
}

void Platform::setMultiSampling(bool enabled)
{
    GP_ERROR("Fix me !");
}

bool Platform::isMultiSampling()
{
    GP_ERROR("Fix me !");
    return false;
}

void Platform::setMultiTouch(bool enabled)
{
    __multiTouch = enabled;
}

bool Platform::isMultiTouch()
{
    return __multiTouch;
}

bool Platform::hasMouse()
{
#ifdef GP_PLATFORM_LINUX
    return true;
#elif GP_PLATFORM_WINDOWS
    return true;
#elif GP_PLATFORM_MACOS
    return true;
#elif GP_PLATFORM_ANDROID
    return false;
#elif GP_PLATFORM_IOS
    return false;
#else
    GP_ERROR("Fix me !");
    return false;
#endif
}

void Platform::setMouseCaptured(bool captured)
{
    if (captured != __mouseCaptured)
    {
        if (captured)
        {
            // Hide the cursor and warp it to the center of the screen
            __mouseCapturePointX = getDisplayWidth() / 2;
            __mouseCapturePointY = getDisplayHeight() / 2;

            setCursorVisible(false);
            SDL_CaptureMouse(SDL_TRUE);
        }
        else
        {
            // Restore cursor
            setCursorVisible(true);
            SDL_CaptureMouse(SDL_FALSE);
        }

        __mouseCaptured = captured;
    }
}

bool Platform::isMouseCaptured()
{
    return __mouseCaptured;
}

void Platform::setCursorVisible(bool visible)
{
    if (visible != __cursorVisible)
    {
        if (visible==false)
        {
            SDL_ShowCursor(false);
        }
        else
        {
            SDL_ShowCursor(true);
        }

        __cursorVisible = visible;
    }
}

bool Platform::isCursorVisible()
{
    return __cursorVisible;
}

bool Platform::hasAccelerometer()
{
#ifdef GP_PLATFORM_LINUX
    return false;
#elif GP_PLATFORM_WINDOWS
    return false;
#elif GP_PLATFORM_MACOS
    return false;
#elif GP_PLATFORM_ANDROID
    return true;
#elif GP_PLATFORM_IOS
    return true;
#else
    GP_ERROR("Fix me !");
    return false;
#endif
}

void Platform::getAccelerometerValues(float* pitch, float* roll)
{
#if defined(GP_PLATFORM_LINUX) || defined(GP_PLATFORM_WINDOWS) || defined(GP_PLATFORM_MACOS)
    GP_ASSERT(pitch);
    GP_ASSERT(roll);
    *pitch = 0;
    *roll = 0;
#elif GP_PLATFORM_ANDROID
    GP_ERROR("Fix me !");
#elif GP_PLATFORM_IOS
    GP_ERROR("Fix me !");
#else
    GP_ERROR("Fix me !");
#endif
}

void Platform::getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ)
{
    GP_ERROR("Fix me !");
}

void Platform::setArguments(int* argc, char*** argv)
{
    __app_argc = *argc;
    __app_argv = *argv;
}

void Platform::getArguments(int* argc, char*** argv)
{
    if (argc)
        *argc = __app_argc;
    if (argv)
        *argv = __app_argv;
}

void Platform::displayKeyboard(bool display)
{
#if defined(GP_PLATFORM_LINUX) || defined(GP_PLATFORM_WINDOWS) || defined(GP_PLATFORM_MACOS)
    // Do nothing.
#elif GP_PLATFORM_ANDROID
    GP_ERROR("Fix me !");
#elif GP_PLATFORM_IOS
    GP_ERROR("Fix me !");
#else
    GP_ERROR("Fix me !");
#endif
}

bool Platform::isGestureSupported(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

void Platform::registerGesture(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

void Platform::unregisterGesture(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
}

bool Platform::isGestureRegistered(Gesture::GestureEvent evt)
{
    GP_ERROR("Fix me !");
    return false;
}

bool Platform::launchURL(const char* url)
{
    GP_ERROR("Fix me !");
    return false;
}

std::string Platform::displayFileDialog(size_t mode, const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{
    GP_ERROR("Fix me !");
    return "";
}

void Platform::pollGamepadState(Gamepad* gamepad)
{
    GP_ERROR("Fix me !");
}

void Platform::setWindowSize(int width, int height)
{
    SDL_SetWindowSize(__window, width, height);
    updateWindowSize();
    resizeEventInternal(width, height);
}

void*  Platform::getWindowHandle()
{
    return __window;
}

}
