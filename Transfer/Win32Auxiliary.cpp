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

#endif