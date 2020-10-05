#ifndef LAYOUTSAVER_HPP
#define LAYOUTSAVER_HPP

// // Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "Resource.hpp"
#include <string>

// Forward declarations of functions included in this code module:

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow);

ATOM register_window_class(HINSTANCE instance, LPCWSTR classname, WNDPROC callback);

HWND init_instance(HINSTANCE instance, LPCWSTR classname, LPCWSTR window_title, int nCmdShow);

BOOL CALLBACK EnumGetVisibleWindowPos(_In_ HWND hwnd, _In_ LPARAM userdata);

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

struct win_data
{
    HWND window;
    std::wstring title;
    WINDOWPLACEMENT placement;
};

#endif