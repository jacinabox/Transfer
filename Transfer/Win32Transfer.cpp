
#include "stdafx.h"
#include <list>
#include <iterator>
#include <functional>
#include <memory>
#include "Win32Transfer.h"
#include "Transfer.h"

#ifdef WIN32

using namespace std::placeholders;

static std::list<CWPSTRUCT> messages;
static bool hook_is_set_flag = false;
std::function<Nothing(HDC)> wm_paint_handler(null_sink2<HDC>);

static LRESULT WINAPI hook_proc(LONG code, WPARAM wParam, LPARAM lParam) {
	messages.push_back(*reinterpret_cast<CWPSTRUCT*>(lParam));

	return CallNextHookEx(NULL, code, wParam, lParam);
}

class Win32Transfer : public Transfer<Nothing, MSG> {
public:
	Win32Transfer() {
	}
	virtual ~Win32Transfer() {
	}
	virtual std::auto_ptr<Transfer<Nothing, MSG> > transduce(const Nothing& dummy_input,
		std::function<void(const MSG&)>& sink) const {
		
		MSG msg, msg2;
		std::list<CWPSTRUCT>::iterator it;

		if (!GetMessage(&msg, NULL, 0, 0)) exit(0);

		while ((it=messages.begin()) != messages.end()) {
			msg2.hwnd = it->hwnd;
			msg2.message = it->message;
			msg2.wParam  = it->wParam;
			msg2.lParam = it->lParam;
			
			sink(msg2);

			messages.pop_front();
		}

		//It is ...
		sink(msg);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
		return std::auto_ptr<Transfer<Nothing, MSG> >(0);
	}
	virtual Transfer<Nothing, MSG>* clone() const {
		return new Win32Transfer();
	}
	virtual void print_tr_structure() const {
		std::cout << "win32_source()";
	}
};

//It constructs a transfer that retrieves messages from the queue of messages coming-
//into a Windows application. It also discharges the duties of a message loop, dispatching
//messages. Note: this type of transfer blocks the caller, and therefore can be
//used fruitfully in one place in a transfer network.
Transfer<Nothing, MSG>& win32_source() {
	if (!hook_is_set_flag) {
		SetWindowsHookEx(WH_CALLWNDPROC, reinterpret_cast<HOOKPROC>(hook_proc), NULL, NULL);
		hook_is_set_flag = true;
	}
	
	return *new Win32Transfer();
}

////////////////////////////////

bool filter_helper1(HWND hwnd, MSG msg) {
	return hwnd == msg.hwnd;
}

Transfer<MSG, MSG>& filter_hwnd(HWND hwnd) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper1, hwnd, _1));

	return filter(predicate);
}

bool filter_helper2(int code, MSG msg) {
	return code == msg.message;
}

Transfer<MSG, MSG>& filter_code(int code) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper2, code, _1));
	
	return filter(predicate);
}

LRESULT send_helper(MSG msg) {
	return SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

Transfer<MSG, LRESULT>& send_message() {
	return map(send_helper);
}

BOOL post_helper(MSG msg) {
	return PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

Transfer<MSG, BOOL>& post_message() {
	return map(post_helper);
}

static BOOL set_window_text_helper(HWND hwnd, std::basic_string<char> s) {
	return SetWindowTextA(hwnd, s.c_str());
}

Transfer<std::basic_string<char>, BOOL>& set_window_text(HWND hwnd) {
	std::function<BOOL(std::basic_string<char>)> f(std::bind(set_window_text_helper, hwnd, _1));

	return map(f);
}

static std::basic_string<char> get_window_text_helper(HWND hwnd, Nothing _dummy_input) {
	char buffer[1024];

	GetWindowTextA(hwnd, buffer, sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	return std::string(buffer, sizeof(buffer));
}

Transfer<Nothing, std::basic_string<char> >& get_window_text(HWND hwnd) {
	std::function<std::basic_string<char>(Nothing)> f(std::bind(get_window_text_helper, hwnd, _1));

	return map(f);
}

///////////////////////////////////////

const static TCHAR* w_class_name = _T("_TR_FRAME_WINDOW");

static LRESULT CALLBACK frame_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC dc;

	switch (message) {
	case WM_PAINT:
		dc = BeginPaint(hwnd, &ps);

		//Call user defined-paint function.
		wm_paint_handler(dc);

		EndPaint(hwnd, &ps);

		break;
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	case WM_NCDESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND create_frame_window(LPCTSTR title, HICON icon, HMENU menu) {
	HINSTANCE inst;
	WNDCLASS wndclass;

	inst = GetModuleHandle(NULL);
	wndclass.style = 0;
	wndclass.lpfnWndProc = frame_window_proc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = inst;
	wndclass.hIcon = icon;
	wndclass.hCursor = LoadCursor(inst, IDC_ARROW);
	wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = w_class_name;
	
	RegisterClass(&wndclass);
	return CreateWindowEx(0, w_class_name, title, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, menu,
		inst, NULL);

}
/////////////////////////////////

static Nothing fill_rect_helper(LPCRECT pRect, HBRUSH hBrush, HDC dc) {
	FillRect(dc, pRect, hBrush);
	return Nothing();
}

std::function<Nothing(HDC)> fill_rect(LPCRECT pRect, HBRUSH hBrush) {
	return std::bind(fill_rect_helper, pRect, hBrush, _1);
}

static Nothing rectangle_helper(LPCRECT pRect, HPEN hPen, HBRUSH hBrush, HDC dc) {
	HGDIOBJ hOldPen = SelectObject(dc, hPen);
	HGDIOBJ hOldBrush = SelectObject(dc, hBrush);

	Rectangle(dc, pRect->left, pRect->top, pRect->right, pRect->bottom);
	SelectObject(dc, hOldPen);
	SelectObject(dc, hOldBrush);
	return Nothing();
}

std::function<Nothing(HDC)> rectangle(LPCRECT pRect, HPEN hPen, HBRUSH hBrush) {
	return std::bind(rectangle_helper, pRect, hPen, hBrush, _1);
}

static Nothing move_to_helper(const POINT* pPoint, HDC dc) {
	MoveToEx(dc, pPoint->x, pPoint->y, NULL);
	return Nothing();
}

DRAW_FUNCTION move_to(const POINT* pPoint) {
	return std::bind(move_to_helper, pPoint, _1);
}

static Nothing line_to_helper(const POINT* pPoint, HPEN hPen, HDC dc) {
	HGDIOBJ hOldPen = SelectObject(dc, hPen);

	LineTo(dc, pPoint->x, pPoint->y);
	SelectObject(dc, hOldPen);
	return Nothing();
}

DRAW_FUNCTION line_to(const POINT* pPoint, HPEN hPen) {
	return std::bind(line_to_helper, pPoint, hPen, _1);
}

///std::

#endif