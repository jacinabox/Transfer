#ifndef _WIN32_SOURCE
#define _WIN32_SOURCE

//Enable on Win32 only.
#ifdef WIN32

#include <windows.h>
#include <tchar.h>
#include <utility>
#include <functional>
#include <string>

//It constructs a transfer that retrieves messages from the queue of messages coming-
//into a Windows application. It also discharges the duties of a message loop, dispatching
//messages. Note: this type of transfer blocks the caller, and therefore can be
//used fruitfully once only in a transfer network. Also, the fact that this transfer
//blocks, implies that a non-blocking event source, such as a constant function,
//can be used fruitfully.
Transfer<Nothing, MSG>& win32_source();

//Miscellaneous helpers which are used for processing messages.
Transfer<MSG, MSG>& filter_hwnd(HWND hwnd);
Transfer<MSG, MSG>& filter_code(int code);
//Filter out messages the codes of which lie outside of the half-open
//range [code1,code2). This is occasionally useful when handling
//user-defined message codes.
Transfer<MSG, MSG>& filter_code_in_range(int code1, int code2);
Transfer<MSG, MSG>& filter_wparam(WPARAM wParam);
Transfer<MSG, MSG>& filter_lparam(LPARAM lParam);

Transfer<MSG, LRESULT>& send_message();
Transfer<MSG, BOOL>& post_message();

//Helpers for manipulating window attributes.
Transfer<std::basic_string<char>, BOOL>& set_window_text(HWND hwnd, int id=0);
Transfer<Nothing, std::basic_string<char> >& get_window_text(HWND hwnd, int id=0);
Transfer<int, Nothing>& set_selected_item(HWND hListControl);
Transfer<Nothing, HWND>& get_dlg_item(HWND hwnd, int id);
Transfer<std::pair<HWND, RECT>, Nothing>& resize_window();
Transfer<RECT, Nothing>& resize_window2(HWND hWnd);
Transfer<Nothing, SCROLLINFO>& get_scroll_info(int nBar, HWND hwnd, int id = 0);
Transfer<SCROLLINFO, Nothing>& set_scroll_info(int nBar, HWND hwnd, int id = 0);

//A helper function to construct a frame window with reasonable defaults.
HWND create_frame_window(LPCTSTR title, HICON icon=0, HMENU menu=0, UINT style=WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);


//Helper functions to create controls and retrieve their handles by id.
struct WINDOW_INFO {
	char class_name[128];
	RECT rect;
	LPCSTR text;
	int id;
	HWND hWndParent;
};

Transfer<WINDOW_INFO, HWND>& create_control();
Transfer<Nothing, HWND>& create_control2(char class_name[128],
	RECT rect,
	LPCSTR text,
	int id,
	HWND hWndParent);


//Drawing....
extern std::function<Nothing(HDC)> wm_paint_handler;

template<class FUNCTIONAL> Nothing handle_wm_paint_helper(std::pair<FUNCTIONAL, MSG> pair) {
	PAINTSTRUCT ps;
	HDC dc;
	
	if (pair.second.message == WM_PAINT) {
		wm_paint_handler = pair.first;
	}

	return Nothing();
}

template<class FUNCTIONAL> Transfer<std::pair<FUNCTIONAL, MSG>, Nothing>& handle_wm_paint() {
	return map(handle_wm_paint_helper);
}

template<class FUNCTIONAL> Nothing handle_wm_paint_helper2(FUNCTIONAL functional, MSG msg) {
	PAINTSTRUCT ps;
	HDC dc;

	if (msg.message == WM_PAINT) {
		wm_paint_handler = functional;
	}

	return Nothing();
}

template<class FUNCTIONAL> Transfer<MSG, Nothing>& handle_wm_paint2(FUNCTIONAL functional) {
	std::function<Nothing(MSG)> _f(std::bind(handle_wm_paint_helper2<FUNCTIONAL>, functional, _1));
	
	return map(_f);
}

template<class T, class U, class RHS_FUNCTIONAL> T compose_fn_helper(std::function<T(U)> f1, RHS_FUNCTIONAL f2, U x) {
	f1(x);
	return f2(x);
}

typedef std::function<Nothing(HDC)> DRAW_FUNCTION;

//Functions of one argument, compose similarly as the reader modality, although of course
//there are additional effects stemming from C++ I/O. I use function composition
//operator to build a mini drawing DSL, which is invoked with handle_wm_paint_helper.
//It can be said that this definition takes two DRAW_FUNCTIONs as arguments,
//and returns a composite DRAW_FUNCTION.
template<class T, class U, class RHS_FUNCTIONAL> std::function<T(U)> operator >>(std::function<T(U)> f1, RHS_FUNCTIONAL f2) {
	std::function<T(U)> _f(std::bind(compose_fn_helper<T, U, RHS_FUNCTIONAL>, f1, f2, _1));

	return _f;
}

DRAW_FUNCTION fill_rect(LPCRECT pRect, HBRUSH hBrush);
DRAW_FUNCTION rectangle(LPCRECT pRect, HPEN hPen, HBRUSH hBrush);
DRAW_FUNCTION move_to(const POINT* pPoint);
DRAW_FUNCTION line_to(const POINT* pPoint, HPEN hPen);
DRAW_FUNCTION ellipse(LPCRECT pRect, HPEN hPen, HBRUSH hBrush);

#endif

//Interface to the framework's hook system.
void set_window_hook(HWND hWnd, const std::function<void(const CWPSTRUCT*)>& f);

//Retrieve the schedule object associated with some frame window.
//Returns null if the window is not associated with a schedule.
  Schedule* retrieve_schedule(HWND hWnd);

//Set loop protect.
void set_loop_protect(bool _loop_protect_flag);

#endif