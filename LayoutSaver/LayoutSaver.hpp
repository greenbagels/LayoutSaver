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

// Forward declarations of functions included in this code module:
ATOM                RegisterWindowClass(HINSTANCE, LPCWSTR);
BOOL                InitInstance(HINSTANCE, LPCWSTR, LPCWSTR, int);
BOOL CALLBACK		EnumGetVisibleWindowPos(_In_ HWND hwnd, _In_ LPARAM userdata);
BOOL CALLBACK		EnumRestoreVisibleWindowPos(_In_ HWND hwnd, _In_ LPARAM userdata);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT             SaveWindows(HWND);
LRESULT             SetWindows(HWND);

#endif