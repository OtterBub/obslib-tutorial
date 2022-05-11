//
// VIDBOX inc. SKPark (SungKyoung Park) 
// CreatedDate: May-10-2022
// 
// description
// This Source is View Source's Properties
//
// 2. View Properties

#include <iostream>
#include <string>

// include for create window
#include <Windows.h>

// include obs lib
#include <obs.hpp>

// input obslib
#pragma comment(lib, "obs.lib")

// windows title name
static TCHAR szTitle[] = L"2.ViewProperties";

// for window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// create window (for draw scene)
inline HWND CreateMainWindow();

// this used function when Create Display
static void RenderWindow(void* data, uint32_t cx, uint32_t cy);

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

    // obs reset video
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

    // create display
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

    obs_set_output_source(0, obs_scene_get_source(scene));


    //////////////////////////////////////
    // now, view properties.
    //////////////////////////////////////
    std::cout << "\n\n --- now, view properties." << std::endl;

    // ----- 1. Get Source Properties -----
    obs_properties_t* props = obs_get_source_properties("dshow_input");

    // ----- 2. Get Properties's first property -----
    obs_property_t* prop = obs_properties_first(props);
    bool enumRun = true;
    while (enumRun) {
        
        // get property name
        std::string propName = obs_property_name(prop);

        // get property type
        obs_property_type type = obs_property_get_type(prop);

        std::cout << "property Name: " << propName << std::endl;
        std::cout << "obs_property_type: ";
        switch(type) {
            case OBS_PROPERTY_INVALID:
                std::cout << "INVALID";
                break;

            case OBS_PROPERTY_BOOL:
                std::cout << "BOOL";
                break;

            case OBS_PROPERTY_INT:
                std::cout << "INT";
                break;

            case OBS_PROPERTY_FLOAT:
                std::cout << "FLOAT";
                break;

            case OBS_PROPERTY_TEXT:
                std::cout << "TEXT";
                break;

            case OBS_PROPERTY_PATH:
                std::cout << "PATH";
                break;

            case OBS_PROPERTY_COLOR:
                std::cout << "COLOR";
                break;

            case OBS_PROPERTY_BUTTON:
                std::cout << "BUTTON";
                break;

            case OBS_PROPERTY_FONT:
                std::cout << "FONT";
                break;

            case OBS_PROPERTY_LIST:
            case OBS_PROPERTY_EDITABLE_LIST:
                if(type == OBS_PROPERTY_LIST)
                    std::cout << "LIST" << std::endl;
                else if(type == OBS_PROPERTY_EDITABLE_LIST)
                    std::cout << "EDITABLE_LIST" << std::endl;
                {
                    obs_combo_type comboType = obs_property_list_type(prop);
                    obs_combo_format comboFormat = obs_property_list_format(prop);
                    size_t listSize = obs_property_list_item_count(prop);

                    std::cout << "- List Type: ";
                    switch (comboType) {
                    case obs_combo_type::OBS_COMBO_TYPE_INVALID:
                        std::cout << "INVALID";
                        break;
                    case obs_combo_type::OBS_COMBO_TYPE_EDITABLE:
                        std::cout << "EDITABLE";
                        break;
                    case obs_combo_type::OBS_COMBO_TYPE_LIST:
                        std::cout << "LIST";
                        break;
                    }
                    std::cout << std::endl;


                    std::cout << "- List Format: ";
                    switch (comboFormat) {
                    case obs_combo_format::OBS_COMBO_FORMAT_INVALID:
                        std::cout << "INVALID";
                        break;

                    case obs_combo_format::OBS_COMBO_FORMAT_INT:
                        std::cout << "INT";
                        break;

                    case obs_combo_format::OBS_COMBO_FORMAT_FLOAT:
                        std::cout << "FLOAT";
                        break;

                    case obs_combo_format::OBS_COMBO_FORMAT_STRING:
                        std::cout << "STRING";
                        break;
                    }
                    std::cout << std::endl;
                    
                    for (size_t i = 0; i < listSize; i++) {
                        std::cout << "- [" << i << "]" << "Value: ";
                        switch (comboFormat) {
                        case obs_combo_format::OBS_COMBO_FORMAT_INVALID:
                            break;

                        case obs_combo_format::OBS_COMBO_FORMAT_INT:
                            {
                                long long value = obs_property_list_item_int(prop, i);
                                std::cout << value;
                            }
                            break;

                        case obs_combo_format::OBS_COMBO_FORMAT_FLOAT:
                            {
                                double value = obs_property_list_item_float(prop, i);
                                std::cout << value;
                            }
                            break;

                        case obs_combo_format::OBS_COMBO_FORMAT_STRING:
                            {
                                std::string value = obs_property_list_item_string(prop, i);
                                std::cout << value;
                            }
                            break;
                        }
                        std::cout << std::endl;
                    }
                }
                break;

            case OBS_PROPERTY_FRAME_RATE:
                std::cout << "FRAME_RATE";
                break;

            case OBS_PROPERTY_GROUP:
                std::cout << "GROUP";
                break;

            case OBS_PROPERTY_COLOR_ALPHA:
                std::cout << "COLOR_ALPHA";
                break;
        }
        std::cout << std::endl;

        // return false if the last property
        enumRun = obs_property_next(&prop);
        std::cout << std::endl;
    }

    
    


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
    case WM_KEYDOWN:
        
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