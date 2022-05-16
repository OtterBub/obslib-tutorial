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

// for camera list
#include <vector>

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

// Camera List
std::vector<std::string> camList;

// main function
int main()
{
    obs_startup("en-US", NULL, NULL);

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
    
    OBSScene scene = obs_scene_create("main scene");

    OBSSource source = obs_source_create(
        "dshow_input", // from obs module (win-dshow.dll)
        "Video Capture", // source name
        NULL, // source setting
        NULL // source hotkey
    );

    obs_scene_add(scene, source);

    obs_set_output_source(0, obs_scene_get_source(scene));

    obs_properties_t* props = obs_source_properties(source);
    obs_property_t* prop = obs_properties_get(props, "video_device_id");
    OBSData data = obs_source_get_settings(source);
    size_t count = obs_property_list_item_count(prop);

    for (size_t index = 0; index < count; index++) {
        camList.push_back(obs_property_list_item_string(prop, index));
    }

    // set data value
    if(!camList.empty())
        obs_data_set_string(data, "video_device_id", camList[0].c_str());
    
    // source apply data
    obs_source_update(source, data);

    ////////////////////////////////////////////////////////
    // ----- Webcam display view fit the window size -----
    ////////////////////////////////////////////////////////

    // get item
    OBSSceneItem item = obs_scene_sceneitem_from_source(scene, source);

    // set bounds width, height
    vec2 bounds;
    vec2_set(&bounds, width, height);

    // set bounds to item
    obs_sceneitem_set_bounds(item, &bounds);

    // set bounds type
    obs_sceneitem_set_bounds_type(item, obs_bounds_type::OBS_BOUNDS_SCALE_INNER);



    // Window loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}




// for window
// for window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam >= 0x30 &&
            wParam <= 0x39)
        {
            int numKey = wParam - 0x30;
            if (!camList.empty() && numKey < camList.capacity())
            {
                OBSSource camSource = obs_get_source_by_name("Video Capture");
                OBSData camData = obs_source_get_settings(camSource);

                std::cout << "[" << numKey << "] " << camList[numKey] << std::endl;
                obs_data_set_string(camData, "video_device_id", camList[numKey].c_str());

                obs_source_update(camSource, camData);
            }
        }
        break;

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