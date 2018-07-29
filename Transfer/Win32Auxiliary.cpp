#include "stdafx.h"
#include <windows.h>
#include <functional>

#ifdef WIN32

static bool get_edit_control_changes_helper(MSG msg) {
	return HIWORD(msg.wParam) == EN_CHANGE;
}

Transfer<MSG, std::basic_string<char> >& get_edit_control_changes(HWND hEditControl) {
	return filter_code(WM_COMMAND) >>
		filter(get_edit_control_changes_helper) >>
		filter_lparam(reinterpret_cast<LPARAM>(hEditControl)) >>
		map(null_sink2<MSG>) >>
		get_window_text(hEditControl);
}

static RECT get_window_rectangle_changes_helper(MSG msg) {
	


	RECT rect;
	WINDOWPOS* wpp = reinterpret_cast<WINDOWPOS*>(msg.lParam);

	rect.left = wpp->x;
	rect.top = wpp->y;
	rect.right = wpp->x + wpp->cx;
	rect.bottom = wpp->y + wpp->cy;

	return rect;
}

Transfer<MSG, RECT>& get_window_rectangle_changes(HWND hWnd) {
	

	
	
	return filter_code(WM_WINDOWPOSCHANGED) >>
		filter_hwnd(hWnd) >>
		map(get_window_rectangle_changes_helper);
}

static LRESULT get_list_control_changes_helper(MSG msg) {
	return SendMessage(reinterpret_cast<HWND>(msg.lParam), LB_GETSEL, 0, 0);
}

static bool get_list_control_changes_helper2(MSG msg) {
	return HIWORD(msg.wParam) == LBN_SELCHANGE;
}

Transfer<MSG, LRESULT>& get_list_control_changes(HWND hListControl) {
	

	return filter_code(WM_COMMAND) >>
		filter(get_list_control_changes_helper2) >>
		filter_lparam(reinterpret_cast<LPARAM>(hListControl)) >>
		map(get_list_control_changes_helper);
}


///////////////////////////////////////

std::pair<HWND, RECT> size_to_parent_helper(HWND hWndParent, MSG msg) {
	//WINDOWPOS* wpp = reinterpret_cast<WINDOWPOS*>(msg.lParam);
	RECT rt;

	/*rt.left = wpp->x;
	rt.top = wpp->y;
	rt.right = wpp->x + wpp->cx;
	rt.bottom = wpp->y + wpp->cy;*/
	GetClientRect(hWndParent, &rt);

	return std::make_pair(GetWindow(hWndParent, GW_CHILD), rt);
}

/*bool size_to_parent_helper2(HWND hWndParent, MSG msg) {
return GetWindow(hWndParent, GW_CHILD) == msg.hwnd;
}*/

Transfer<MSG, Nothing>& size_to_parent(HWND hWndParent) {
	HWND hWnd = GetWindow(hWndParent, GW_CHILD);
	std::function<std::pair<HWND, RECT>(MSG)> _f(std::bind(size_to_parent_helper, hWndParent, _1));

	return filter_code(WM_WINDOWPOSCHANGED) >> // %
		filter_hwnd(hWndParent) >>
		map(_f) >>
		resize_window();
}

#endif