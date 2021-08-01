// LayoutSaver.cpp : Defines the entry point for the application.

// Use Unicode expansions
#define UNICODE

// Allow us to use window themes
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "core.hpp"
#include "main_window.hpp"

#include <cassert>
#include <string>
#include <vector>
#include <memory>

// List of userspace windows for resizing
// std::vector<struct win_data> window_list;

int APIENTRY wWinMain(_In_ HINSTANCE instance,
                     _In_opt_ HINSTANCE prev_instance,
                     _In_ LPWSTR    cmd_line,
                     _In_ int       cmd_show)
{
    UNREFERENCED_PARAMETER(prev_instance);
    UNREFERENCED_PARAMETER(cmd_line);

    // Cap string lengths to avoid overflow
    const auto MAX_LEN = 100u;
    // Strings holding application information
    std::wstring application_title;
    std::wstring main_wclass_name;
    std::vector<struct win_data> window_list;

    // Initialize global strings
    application_title.resize(MAX_LEN);
    main_wclass_name.resize(MAX_LEN);
    LoadString(instance, IDS_APP_TITLE, &application_title[0], MAX_LEN);
    LoadString(instance, IDC_LAYOUTSAVER, &main_wclass_name[0], MAX_LEN);
    register_window_class(instance, main_wclass_name.c_str(), main_window_callback);

    // Perform application initialization:
    HWND main_window = init_instance(instance,
                                     main_wclass_name.c_str(),
                                     application_title.c_str(),
                                     cmd_show);
    if (!main_window)
    {
        return FALSE;
    }

    SetWindowLongPtr(main_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&window_list));

    auto accelerator_table = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_LAYOUTSAVER));

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, accelerator_table, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM register_window_class(HINSTANCE instance, LPCWSTR classname, WNDPROC callback)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = 0; // CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = callback;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 16 * sizeof(void*);
    wcex.hInstance = instance;
    wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_LS));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_LAYOUTSAVER);
    wcex.lpszClassName = classname;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LS_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND init_instance(HINSTANCE instance, LPCWSTR classname, LPCWSTR window_title, int nCmdShow)
{
   auto style = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
   // TODO: Parameterize these
   const auto width = 200, height = 120;
   auto top_window = CreateWindow(classname, window_title, style,
      CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);

   if (!top_window)
   {
      return nullptr;
   }

   style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON;
   auto savebutton = CreateWindow(L"BUTTON", L"Save", style,
       40, 20, 40, 25, top_window, nullptr, instance, nullptr);
   auto loadbutton = CreateWindow(L"BUTTON", L"Load", style,
       120, 20, 40, 25, top_window, nullptr, instance, nullptr);

   if (!savebutton)
   {
       return nullptr;
   }

   ShowWindow(top_window, nCmdShow);
   UpdateWindow(top_window);

   return top_window;
}

BOOL CALLBACK EnumGetVisibleWindowPos(_In_ HWND window, _In_ LPARAM user_data)
{
    struct win_data data;
    data.placement.length = sizeof(WINDOWPLACEMENT);
    auto title_len = GetWindowTextLength(window);
    data.title.resize(title_len+1);
    // Windows desktops typically have a lot of hidden or titleless windows;
    // to be safe, we're going to assume the user (by default) does NOT want
    // us to touch those windows.
    if (!(GetWindowText(window, &data.title[0], title_len+1) && IsWindowVisible(window)))
    {
        if (GetLastError())
        {
            // Stop, we encountered an error
            return FALSE;
        }
        // Skip because empty title
        return TRUE;
    }
    if (!GetWindowPlacement(window, &data.placement))
    {
        return FALSE;
    }
    using vecptr = std::vector<struct win_data>*;
    auto window_list = reinterpret_cast<vecptr>(user_data);
    data.window = window;
    window_list->emplace_back(data);
    return TRUE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}