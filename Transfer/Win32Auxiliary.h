#ifndef _WIN32_AUXILIARY
#define _WIN32_AUXILIARY

#ifdef WIN32

#include <utility>
#include <string>

Transfer<MSG, std::basic_string<char> >& get_edit_control_changes(HWND hEditControl);
Transfer<MSG, RECT>& get_window_rectangle_changes(HWND hWnd);
Transfer<MSG, LRESULT>& get_list_control_changes(HWND hListControl);

Transfer<MSG, Nothing>& size_to_parent(HWND hWndParent);
//The Z-order of the parent window's child controls, determines their order in the layout.
Transfer<MSG, Nothing>& size_children_according_to_layout(HWND hWndParent);

/*HWND make_list_view_control(HWND hWndParent, std::function<void()>);

}*/

#endif

#endif