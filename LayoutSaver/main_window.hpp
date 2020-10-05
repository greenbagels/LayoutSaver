#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <Windows.h>
#include "Resource.hpp"

LRESULT CALLBACK main_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
int hide_to_notification_area(HWND window, UINT icon_uid, UINT icon_resource_id);
int restore_from_notification_area(HWND window, UINT icon_uid);

#endif