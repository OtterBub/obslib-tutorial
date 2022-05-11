//
// VIDBOX inc. SKPark (SungKyoung Park) 
// CreatedDate: May-10-2022
// 
// description
// This Source is only Display Camera
//
// 3. Item Set Bound

#include <iostream>
#include <string>

// include for create window
#include <Windows.h>

// include obs lib
#include <obs.hpp>

// input obslib
#pragma comment(lib, "obs.lib")

// windows title name
static TCHAR szTitle[] = L"3.ItemSetBound";

// for window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// create window (for draw scene)
inline HWND CreateMainWindow();

// this used function when Create Display
static void RenderWindow(void* data, uint32_t cx, uint32_t cy);

// main function
int main()
{
    // ----- 1. start up obslib -----
    obs_startup("en-US", NULL, NULL);

    // ----- 2. Load Modules -----

    // set modules path
    std::string pluginsPath = "\\..\\..\\data\\obs-plugins\\";
    std::string dataPath = "\\..\\..\\data\\";
    std::string currentPath = "";
#ifdef _DEBUG
    pluginsPath += "Debug\\";
#else
    pluginsPath += "Release\\";
#endif

    // get current folder path
    char buff[MAX_PATH];
    GetModuleFileNameA(NULL, buff, sizeof(buff));
    std::string::size_type pos = std::string(buff).find_last_of("\\/");
    currentPath = std::string(buff).substr(0, pos);

    pluginsPath = currentPath + pluginsPath;
    dataPath = currentPath + dataPath;


    std::cout << "currentPath: " << currentPath << std::endl;
    std::cout << "pluginsPath: " << pluginsPath << std::endl;
    std::cout << "dataPath: " << dataPath << std::endl;

    // add modules path
    obs_add_module_path(pluginsPath.c_str(), dataPath.c_str());

    // load modules
    obs_load_all_modules();

    // show loaded modules
    obs_log_loaded_modules();

    // Create Window for draw scene
    HWND mainWindow = CreateMainWindow();
    RECT winrect;
    GetWindowRect(mainWindow, &winrect);

    // ----- 3. obs reset video -----
    int width = winrect.right - winrect.left;
    int height = winrect.bottom - winrect.top;

    obs_video_info ovi;
    ovi.adapter = 0;
    ovi.base_width = width; //width
    ovi.base_height = height; //height
    ovi.fps_num = 30000;
    ovi.fps_den = 1001;
    ovi.graphics_module = "libobs-d3d11"; // libobs-d3d11 or libobs-opengl
    ovi.output_format = video_format::VIDEO_FORMAT_RGBA;
    ovi.output_width = width;
    ovi.output_height = height;

    obs_reset_video(&ovi);

    // ----- 4. Create Display -----
    gs_init_data info = { 0, };
    info.cx = width;
    info.cy = height;
    info.format = GS_RGBA;
    info.zsformat = GS_ZS_NONE;
    info.window.hwnd = mainWindow;

    OBSDisplay display = obs_display_create(&info, 0);

    // set background color
    obs_display_set_background_color(display, 0);

    // register callback reunder function
    obs_display_add_draw_callback(display, RenderWindow, NULL);
    
    // ----- 5. Create Scene -----
    OBSScene scene = obs_scene_create("main scene");

    // ----- 6. Create Source -----
    OBSSource source = obs_source_create(
        "dshow_input", // from obs module (win-dshow.dll)
        "Vidieo Capture", // source name
        NULL, // source setting
        NULL // source hotkey
    );

    // ----- 7. Add Source to Scene -----
    obs_scene_add(scene, source);

    // ----- 8. Draw Scene -----
    obs_set_output_source(0, obs_scene_get_source(scene));

    //////////////////////////////////////
    // but until doesn't display current.
    // need source set property
    // refer to below continue
    //////////////////////////////////////

    // ----- 9. Get Source Properties -----
    obs_properties_t* props = obs_source_properties(source);
    obs_property_t* prop = obs_properties_get(props, "video_device_id");

    // get camName by index 0
    std::string camName = obs_property_list_item_string(prop, 0);

    // ----- 10. Update Source Property -----
    // get data value by source
    OBSData data = obs_source_get_settings(source);

    // set data value
    obs_data_set_string(data, "video_device_id", camName.c_str());
    
    // source apply data
    obs_source_update(source, data);


    // Window loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}




// for window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// create window (for draw scene)
inline HWND CreateMainWindow()
{
    TCHAR szWindowClass[] = L"obslibApp";

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            L"Call to RegisterClassEx failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return NULL;
    }

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 300,
        NULL,
        NULL,
        wcex.hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            L"Call to CreateWindowEx failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return NULL;
    }

    // show window
    ShowWindow(hWnd, SW_RESTORE);
    UpdateWindow(hWnd);

    return hWnd;
}

// this used function when Create Display
static void RenderWindow(void* data, uint32_t cx, uint32_t cy)
{
    // draw Source on main Scene
    obs_render_main_texture();

    UNUSED_PARAMETER(data);
    UNUSED_PARAMETER(cx);
    UNUSED_PARAMETER(cy);
}