
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

		if (!GetMessage(&msg,NULL,0,0)) throw "Received WM_QUIT message";

		while ((it=messages.begin()) != messages.end()) {
			msg2.hwnd = it->hwnd;
			msg2.message = it->message;
			msg2.wParam  = it->wParam;
			msg2.lParam = it->lParam;
			
			sink(msg2);

			messages.pop_front();
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		sink(msg);
		
		return std::auto_ptr<Transfer<Nothing, MSG> >(0);
	}
	virtual Transfer<Nothing, MSG>* clone() const {
		return new Win32Transfer();
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

BOOL set_window_text_helper(HWND hwnd, LPTSTR s) {
	return SetWindowText(hwnd, s);
}

Transfer<LPTSTR, BOOL>& set_window_text(HWND hwnd) {
	std::function<BOOL(LPTSTR)> f(std::bind(set_window_text_helper, hwnd, _1));

	return map(f);
}

///////////////////////////////////////

const static TCHAR* w_class_name = _T("_TR_FRAME_WINDOW");

HWND create_frame_window(LPCTSTR title, HICON icon, HMENU menu) {
	HINSTANCE inst;
	WNDCLASS wndclass;

	inst = GetModuleHandle(NULL);
	wndclass.style = 0;
	wndclass.lpfnWndProc = DefWindowProc;
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

#endif