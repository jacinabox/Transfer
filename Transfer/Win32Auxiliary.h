#ifndef _WIN32_AUXILIARY
#define _WIN32_AUXILIARY

#ifdef WIN32

#include <utility>
#include <string>

Transfer<MSG, std::basic_string<char> >& get_edit_control_changes(HWND hEditControl);
Transfer<MSG, RECT>& get_window_rectangle_changes(HWND hWnd);
Transfer<MSG, LRESULT>& get_list_control_changes(HWND hListControl);

Transfer<MSG, Nothing>& size_to_parent(HWND hWndParent);

#endif

#endif