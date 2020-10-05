#include "core.hpp"
#include "main_window.hpp"
#include <cassert>
#include <string>
#include <vector>
#include <memory>

static const UINT ICON_ID_LS = 1;

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_SYSCOMMAND - Custom system control buttons
//  WM_COMMAND    - process the application menu
//  WM_PAINT      - Paint the main window
//  WM_APP        - Handle user-defined events
//  WM_DESTROY    - post a quit message and return
//
//
LRESULT CALLBACK main_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::wstring control_text(5, 'a');
    int status;

    auto ptr_as_int = GetWindowLongPtrW(window, GWLP_USERDATA);
    auto window_list = reinterpret_cast<std::vector<struct win_data>*>(ptr_as_int);
    // assert(window_list != nullptr);
    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case SC_MINIMIZE:
                {
                    hide_to_notification_area(window, ICON_ID_LS, IDI_LS);
                    break;
                }
                default:
                DefWindowProcW(window, message, wParam, lParam);
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
                        window_list->clear();
                        EnumWindows(EnumGetVisibleWindowPos, ptr_as_int);

                        if (GetLastError())
                        {
                            std::wstring buf;
                            buf.resize(512);
                            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                                           nullptr,
                                           GetLastError(),
                                           0,
                                           &buf[0],
                                           512,
                                           nullptr);
                            MessageBoxW(window, buf.c_str(), nullptr, MB_OK);
                        }
                        else
                        {
                            MessageBoxW(window,
                                        (L"Successfully saved positions of " +
                                            std::to_wstring(window_list->size()) +
                                            L" windows.").c_str(),
                                        nullptr,
                                        MB_OK);
                        }

                        // Handle Save function
                    }

                    else if (control_text[0] == L'L')
                    {
                        MessageBoxW(window,
                                    (L"Restoring " +
                                        std::to_wstring(window_list->size()) +
                                        L" window positions.").c_str(),
                                     nullptr,
                                     MB_OK);
                        for (auto i : *window_list)
                        {
                            SetWindowPlacement(i.window, &i.placement);
                        }
                        return 0;
                    }
                    break;
            }

            switch (LOWORD(wParam))
            {
                case IDM_ABOUT:
                    DialogBoxW(GetModuleHandleW(nullptr),
                               MAKEINTRESOURCE(IDD_ABOUTBOX),
                               window,
                               About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(window);
                    break;
                case ID_FILE_SETTINGS:

                    break;
                default:
                    return DefWindowProc(window, message, wParam, lParam);
            }
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(window, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(window, &ps);
            break;
        }

        case WM_APP:
        {
            switch (lParam)
            {
            case WM_LBUTTONDBLCLK:
            {
    
            }
            break;
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(window, message, wParam, lParam);
    }
    return 0;
}


int hide_to_notification_area(HWND window, UINT icon_uid, UINT icon_resource_id)
{
    NOTIFYICONDATAW icondata;
    icondata.cbSize = sizeof(NOTIFYICONDATAW);
    icondata.hWnd = window;
    icondata.uID = icon_uid;
    icondata.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    icondata.uCallbackMessage = WM_APP;
    icondata.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(icon_resource_id));
    auto str = L"Double-click to restore LayoutSaver!";
    std::wmemset(icondata.szTip, '\0', sizeof(icondata.szTip) / sizeof(*str));
    std::wmemcpy(icondata.szTip, str, std::wcslen(str));
    icondata.uVersion = 0;
    icondata.dwInfoFlags = NIIF_NONE;
    icondata.hBalloonIcon = nullptr;
    icondata.guidItem = { 0,0,0,{0,0,0,0,0,0,0,0} };
    Shell_NotifyIconW(NIM_ADD, &icondata);
    ShowWindow(window, SW_HIDE);
    return 0;
}

int restore_from_notification_area(HWND window, UINT icon_uid)
{
    NOTIFYICONDATAW icondata;
    icondata.cbSize = sizeof(NOTIFYICONDATAW);
    icondata.hWnd = window;
    icondata.uID = icon_uid;
    Shell_NotifyIconW(NIM_DELETE, &icondata);
    ShowWindow(window, SW_SHOW);
    return 0;
}