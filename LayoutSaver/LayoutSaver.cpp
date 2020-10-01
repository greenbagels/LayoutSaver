// LayoutSaver.cpp : Defines the entry point for the application.

// Allow us to use window themes
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "LayoutSaver.hpp"
#include <string>
#include <vector>
#include <memory>

struct win_data
{
    HWND window;
    std::wstring title;
    RECT pos;
};

// List of userspace windows for resizing
std::vector<struct win_data> window_list;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Cap string lengths to avoid overflow
    const auto MAX_LEN = 100u;
    // Pointer to current application instance
    auto instance = hInstance;
    // Strings holding application information
    std::wstring application_title;
    std::wstring main_wclass_name;

    // Initialize global strings
    application_title.resize(MAX_LEN);
    main_wclass_name.resize(MAX_LEN);
    LoadStringW(instance, IDS_APP_TITLE, &application_title[0], MAX_LEN);
    LoadStringW(instance, IDC_LAYOUTSAVER, &main_wclass_name[0], MAX_LEN);
    RegisterWindowClass(instance, main_wclass_name.c_str());

    // Perform application initialization:
    if (!InitInstance (instance, main_wclass_name.c_str(), application_title.c_str(), nCmdShow))
    {
        return FALSE;
    }

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
ATOM RegisterWindowClass(HINSTANCE instance, LPCWSTR classname)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = 0; // CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = instance;
    wcex.hIcon          = LoadIcon(instance, MAKEINTRESOURCE(IDI_LS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAYOUTSAVER);
    wcex.lpszClassName  = classname;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LS_SMALL));

    return RegisterClassExW(&wcex);
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
BOOL InitInstance(HINSTANCE instance, LPCWSTR classname, LPCWSTR window_title, int nCmdShow)
{
   auto style = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
   // TODO: Parameterize these
   const auto width = 200, height = 120;
   auto top_window = CreateWindowW(classname, window_title, style,
      CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);

   if (!top_window)
   {
      return FALSE;
   }

   style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON;
   auto savebutton = CreateWindowW(L"BUTTON", L"Save", style,
       40, 20, 40, 25, top_window, nullptr, instance, nullptr);
   auto loadbutton = CreateWindowW(L"BUTTON", L"Load", style,
       120, 20, 40, 25, top_window, nullptr, instance, nullptr);

   if (!savebutton)
   {
       return FALSE;
   }

   ShowWindow(top_window, nCmdShow);
   UpdateWindow(top_window);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::wstring control_text(5, 'a');
    int status;
    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case SC_MINIMIZE:
                {
                    NOTIFYICONDATAW icondata;
                    icondata.cbSize = sizeof(NOTIFYICONDATAW);
                    icondata.hWnd = hWnd;
                    icondata.uID = 1;
                    icondata.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
                    icondata.uCallbackMessage = WM_APP;
                    icondata.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_LS));
                    auto str = L"Double-click to restore LayoutSaver!";
                    std::wmemset(icondata.szTip, '\0', sizeof(icondata.szTip)/sizeof(*str));
                    std::wmemcpy(icondata.szTip, str, std::wcslen(str));
                    icondata.uVersion = 0;
                    icondata.dwInfoFlags = NIIF_NONE;
                    icondata.hBalloonIcon = nullptr;
                    icondata.guidItem = { 0,0,0,{0,0,0,0,0,0,0,0} };
                    Shell_NotifyIconW(NIM_ADD, &icondata);
                    ShowWindow(hWnd, SW_HIDE);
                    break;
                }
                default:
                    DefWindowProcW(hWnd, message, wParam, lParam);
            }
            break;
        }

        case WM_COMMAND:
        {
            // Parse the menu selections:
            switch (HIWORD(wParam))
            {
                case BN_CLICKED:
                    status = GetWindowTextW((HWND)lParam, &control_text[0], 5);
                    if (control_text[0] == L'S')
                    {
                        // MessageBoxW(hWnd, L"Save pressed!", nullptr, MB_OK);
                        window_list.clear();
                        EnumWindows(EnumGetVisibleWindowPos, 0);
 
                        if (GetLastError())
                        {
                            std::wstring buf;
                            buf.resize(512);
                            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), 0, &buf[0], 512, nullptr);
                            MessageBoxW(hWnd, buf.c_str(), nullptr, MB_OK);
                        }
                        else
                        {
                            MessageBoxW(hWnd, (L"Successfully saved positions of " + std::to_wstring(window_list.size()) + L" windows.").c_str(), nullptr, MB_OK);
                        }
                        
                        // Handle Save function
                    }
                    else if (control_text[0] == L'L')
                    {
                        MessageBoxW(hWnd, (L"Restoring " + std::to_wstring(window_list.size()) + L" window positions.").c_str(), nullptr, MB_OK);
                        for (auto i : window_list)
                        {
                            auto x = i.pos.left;
                            auto y = i.pos.top;
                            auto width = i.pos.right - x;
                            auto height = i.pos.bottom - y;
                            MoveWindow(i.window, x, y, width, height, TRUE);
                        }
                        return 0;
                    }
                    break;
            }
            switch (LOWORD(wParam))
            {
                case IDM_ABOUT:
                    DialogBoxW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
            break;
        }

        case WM_APP:
        {
            switch (lParam)
            {
                case WM_LBUTTONDBLCLK:
                {
                    NOTIFYICONDATAW icondata;
                    icondata.cbSize = sizeof(NOTIFYICONDATAW);
                    icondata.hWnd = hWnd;
                    icondata.uID = 1;
                    Shell_NotifyIconW(NIM_DELETE, &icondata);
                    ShowWindow(hWnd, SW_SHOW);
                }
                break;
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK EnumGetVisibleWindowPos(_In_ HWND hwnd, _In_ LPARAM userdata)
{
    struct win_data data;
    auto len = GetWindowTextLength(hwnd);
    data.title.resize(len+1);
    if (!(GetWindowTextW(hwnd, &data.title[0], len+1) && IsWindowVisible(hwnd)))
    {
        if (GetLastError())
        {
            // Stop, we encountered an error
            return FALSE;
        }
        // Skip because empty title
        return TRUE;
    }
    if (!GetWindowRect(hwnd, &data.pos))
    {
        return FALSE;
    }
    data.window = hwnd;
    window_list.push_back(data);
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


LRESULT SetWindows(HWND window)
{
    return 0;
}