
#include "stdafx.h"
#undef map
#include <list>
#include <iterator>
#include <functional>
#include <memory>
#include <map>
#include <assert.h>
#include "Win32Transfer.h"
#include "Transfer.h"

#ifdef WIN32

//#define ___map_impl(FUNCTIONAL) (map_(make_function(FUNCTIONAL)))

using namespace std::placeholders;

static std::list<CWPSTRUCT> messages;
static bool hook_is_set_flag = false;
//static RECT window_rect = { 0 };
static bool loop_protect_flag = false;
//static std::map<HWND, Nothing> window_is_dialog_mp;
#define SWLP_MAP_INTERFACE std::map<HWND, std::function<void(const CWPSTRUCT*)> >
static SWLP_MAP_INTERFACE window_hook_mp;
#define SWLP_MAP_INTERFACE2 std::map<HWND, Schedule>
static SWLP_MAP_INTERFACE2 window_schedule_mp;

static HFONT default_font = CreateFontA(14,
	0,
	0,
	0,
	FW_NORMAL,
	0,
	0,
	0,
	ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,
	FF_MODERN,
	"Tahoma"
);

std::function<Nothing(HDC)> wm_paint_handler(null_sink2<HDC>);

static LRESULT WINAPI hook_proc(LONG code, WPARAM wParam, LPARAM lParam) {
	CWPSTRUCT* cwp_p = reinterpret_cast<CWPSTRUCT*>(lParam);

	//Loop protection: messages are not recorded when loop protection is enabled.
	//Loop protection takes advantage of the way message sending works in Win32;
	//when user code triggers a sent message, the window procedure is called
	//directly; so I can think of intervals of loop protection as bracketing the call
	//in user code.
	if (!loop_protect_flag)
		messages.push_back(*cwp_p);

	

	//Certain tasks can only be performed within the scope of the window procedure.
	//For this we can use a hook mechanism.
	SWLP_MAP_INTERFACE::iterator it = window_hook_mp.find(cwp_p->hwnd);

	if (it!=window_hook_mp.end()) it->second(cwp_p);


	//MessageBox(NULL, _T("Test"), 0, 0);
	//Let's free up entries of these maps upon encountering WM_NCDESTROY message.
	if (cwp_p->message == WM_NCDESTROY) {
		window_schedule_mp.erase(cwp_p->hwnd);
		window_hook_mp.erase(cwp_p->hwnd);
	}

	return CallNextHookEx(NULL, code, wParam, lParam);

}

static LRESULT WINAPI keyboard_hook_proc(LONG code, WPARAM wParam, LPARAM lParam) {
	CWPSTRUCT cwp;

	cwp.hwnd = 0;
	cwp.message = (lParam & (1 << 31) ? WM_KEYUP : WM_KEYDOWN);
	cwp.wParam = wParam;
	cwp.lParam = lParam;

	messages.push_back(cwp);

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

		while ((it = messages.begin()) != messages.end()) {
			msg2.hwnd = it->hwnd;
			msg2.message = it->message;
			msg2.wParam = it->wParam;
			msg2.lParam = it->lParam;

			sink(msg2);

			messages.pop_front();
		}

		//It is ...
		sink(msg);

		if ((GetWindowLong(msg.hwnd, GWL_STYLE) & WS_CHILD)
			|| !IsDialogMessage(msg.hwnd, &msg)) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
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

		HINSTANCE hInst = GetModuleHandle(NULL);
		DWORD i = GetCurrentThreadId();
		SetWindowsHookEx(WH_CALLWNDPROC, reinterpret_cast<HOOKPROC>(hook_proc), hInst, i);
		SetWindowsHookEx(WH_KEYBOARD, reinterpret_cast<HOOKPROC>(keyboard_hook_proc), hInst, i);
		//int y = GetLastError();

		hook_is_set_flag = true;
	}
	
	return *new Win32Transfer();
}

////////////////////////////////

static bool filter_helper1(HWND hwnd, MSG msg) {
	return hwnd == msg.hwnd;
}

Transfer<MSG, MSG>& filter_hwnd(HWND hwnd) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper1, hwnd, _1));

	return filter(predicate);
}

static bool filter_helper2(int code1, int code2, MSG msg) {
	return code1 <= msg.message && msg.message < code2;
}

Transfer<MSG, MSG>& filter_code(int code) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper2, code, 1+code, _1));
	
	return filter(predicate);
}

Transfer<MSG, MSG>& filter_code_in_range(int code1, int code2) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper2, code1, code2, _1));

	return filter(predicate);
}

static LRESULT send_helper(MSG msg) {
	return SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

Transfer<MSG, LRESULT>& send_message() {
	std::function<LRESULT(MSG)> _f(send_helper);

	return ___map_impl(_f);
}

static BOOL post_helper(MSG msg) {
	return PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

Transfer<MSG, BOOL>& post_message() {
	std::function<BOOL(MSG)> _f(post_helper);

	return ___map_impl(_f);
}

static BOOL set_window_text_helper(HWND hwnd, int id, std::basic_string<char> s) {
	BOOL b;

	if (id) hwnd = GetDlgItem(hwnd, id);
	
	loop_protect_flag = true;

	b = SetWindowTextA(hwnd, s.c_str());

	loop_protect_flag = false;

	return b;
}

Transfer<std::basic_string<char>, BOOL>& set_window_text(HWND hwnd, int id) {
	std::function<BOOL(std::basic_string<char>)> f(std::bind(set_window_text_helper, hwnd, id, _1));

	return ___map_impl(f);
}

static std::basic_string<char> get_window_text_helper(HWND hwnd, int id, Nothing _dummy_input) {
	char buffer[1024];

	if (id) hwnd = GetDlgItem(hwnd, id);

	GetWindowTextA(hwnd, buffer, sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	return std::string(buffer, sizeof(buffer));
}

Transfer<Nothing, std::basic_string<char> >& get_window_text(HWND hwnd, int id) {
	std::function<std::basic_string<char>(Nothing)> f(std::bind(get_window_text_helper, hwnd, id, _1));

	return ___map_impl(f);
}

Transfer<Nothing, HWND>& get_dlg_item(HWND hwnd, int id) {
	std::function<HWND(Nothing)> _f(std::bind(GetDlgItem, hwnd, id));

	return ___map_impl(_f);
}

static bool filter_helper3(WPARAM wParam, MSG msg) {
	return wParam == msg.wParam;
}

Transfer<MSG, MSG>& filter_wparam(WPARAM wParam) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper3, wParam, _1));

	return filter(predicate);
}

static bool filter_helper4(LPARAM lParam, MSG msg) {
	return lParam == msg.lParam;
}

Transfer<MSG, MSG>& filter_lparam(LPARAM lParam) {
	std::function<bool(MSG)> predicate(std::bind(filter_helper4, lParam, _1));

	return filter(predicate);
}


static Nothing resize_window_helper2(HWND hWnd, RECT rect) {
	//Set loop protect.
	loop_protect_flag = true;

	SetWindowPos(hWnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	loop_protect_flag = false;

	return Nothing();
}

Transfer<RECT, Nothing>& resize_window2(HWND hWnd) {
	std::function<Nothing(RECT)> _f(std::bind(resize_window_helper2, hWnd, _1));

	return ___map_impl(_f);
}

static Nothing resize_window_helper(std::pair<HWND, RECT> pair) {
	return resize_window_helper2(pair.first, pair.second);
}

Transfer<std::pair<HWND, RECT>, Nothing>& resize_window() {
	return ___map_impl(make_function(resize_window_helper));
}

Nothing set_selected_item_helper(HWND hListControl, int index) {
	loop_protect_flag = true;
	SendMessage(hListControl, LB_SETCURSEL, index, 0);
	loop_protect_flag = false;
	return Nothing();
}

Transfer<int, Nothing>& set_selected_item(HWND hListControl) {
	std::function<Nothing(int)> _f(std::bind(set_selected_item_helper, hListControl, _1));

	return ___map_impl(_f);
}

///////////////////////////////////////

static LPCTSTR w_class_name = _T("_TR_FRAME_WINDOW");
static HWND hLastFocusWnd = 0;
const static UINT my_timer_id = 1001;
static Schedule schedule;

INT_PTR CALLBACK frame_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC dc;
	WINDOWPOS* wpp;
	unsigned t_interval;

	switch (message) {
	case WM_PAINT:
		dc = BeginPaint(hwnd, &ps);

		//Call user defined-paint function.
		wm_paint_handler(dc);

		EndPaint(hwnd, &ps);

		return TRUE;
	/*case WM_ERASEBKGND:
		return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));*/
		
		
		
		

		
	
		
	/*case WM_GETDLGCODE:
		//Setup an acceptable default policy for handling keyboard input.
		SetWindowLong(hwnd, 0, DLGC_WANTALLKEYS);
		return TRUE;*/
	
	/*case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		return TRUE;*/
	case WM_ACTIVATE:
		SetFocus(hLastFocusWnd);
		return TRUE;
	case WM_SETFOCUS:
		hLastFocusWnd = GetFocus();
		return TRUE;
	case DM_GETDEFID: break;
	case DM_SETDEFID: break;		
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	case WM_TIMER:
		//In theory, long running-operations in the discharge_scheduled_events
		//procedure call could lead to this handler getting invoked again,
		//therefore I have the timer disabled temporarily while going through
		//discharge_scheduled_events.
		KillTimer(hwnd, my_timer_id);
		t_interval = window_schedule_mp.find(hwnd)->second.discharge_scheduled_events();
		if (t_interval != -1) SetTimer(hwnd, my_timer_id, t_interval, NULL);
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;
	case WM_NCDESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

HWND create_frame_window(LPCTSTR title, HICON icon, HMENU menu) {
	HINSTANCE inst;
	//WNDCLASS wndclass;
	DLGTEMPLATE dlgTemplate[5];
	HWND hWnd;

	inst = GetModuleHandle(NULL);

	/*wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; //DLGWINDOWEXTRA;
	wndclass.style = 0;
	wndclass.lpfnWndProc = frame_window_proc;
	wndclass.hInstance = inst;
	wndclass.hIcon = icon;
	wndclass.hCursor = LoadCursor(inst, IDC_ARROW);
	wndclass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = w_class_name;
	
	RegisterClass(&wndclass);*/

	/*return CreateWindowEx(0, w_class_name, title, WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, menu,
		inst, NULL);*/

	dlgTemplate[0].style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	dlgTemplate[0].dwExtendedStyle = 0;
	dlgTemplate[0].cdit = 0;
	dlgTemplate[0].x = CW_DEFAULT;
	dlgTemplate[0].y = CW_DEFAULT;
	dlgTemplate[0].cx = CW_DEFAULT;
	dlgTemplate[0].cy = CW_DEFAULT;

	memset(dlgTemplate + 1, 0, 4 * sizeof(dlgTemplate[0]));

	window_schedule_mp.insert(std::make_pair(hWnd, Schedule()));

	hWnd = CreateDialogIndirectA(inst, dlgTemplate, NULL, frame_window_proc);

	SetWindowText(hWnd, title);
	ShowWindow(hWnd, SW_SHOW);
	return hWnd;

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

static Nothing ellipse_helper(LPCRECT pRect, HPEN hPen, HBRUSH hBrush, HDC dc) {
	HGDIOBJ hOldPen = SelectObject(dc, hPen);
	HGDIOBJ hOldBrush = SelectObject(dc, hBrush);

	Ellipse(dc, pRect->left, pRect->top, pRect->right, pRect->bottom);
	SelectObject(dc, hOldPen);
	SelectObject(dc, hOldBrush);
	return Nothing();
}

std::function<Nothing(HDC)> ellipse(LPCRECT pRect, HPEN hPen, HBRUSH hBrush) {
	return std::bind(ellipse_helper, pRect, hPen, hBrush, _1);
}

/////////////////////////////////////////

/*static RECT get_window_rectangle_changes_helper(std::function<RECT(RECT)> update_f, MSG msg) {
	//Give the update function a chance to *update*, the rectangle, in the manner implied
	//by the documentation for WM_WINDOWPOSCHANGING.


}

Transfer<MSG, RECT>& get_window_rectangle_changes(HWND hWnd, std::function<RECT(RECT)> update_f=std::function<RECT(RECT)>(identity__<RECT>)) {
	std::function<RECT(MSG)> _f(std::bind(get_window_rectangle_changes_helper, update_f, _1));

	//	Something to think about around intercepting and overriding change
	//messages....
	return filter_code(WM_WINDOWPOSCHANGED) >>
		filter_hwnd(hWnd) >>
		map(_f);
}*/

///////////////////////////////////////////

//Idea: In a dialog box, you don't hardly ever want to have two controls with the
//same id. Why not check to see if a control exists with the given id, and if
//not then create it to specifications.

static HWND create_control_helper(WINDOW_INFO window_info) {
	HWND hWnd;

	//loop_protect_flag = true;

	if (!(hWnd = GetDlgItem(window_info.hWndParent, window_info.id))) {
		hWnd = CreateWindowExA(0, window_info.class_name, window_info.text, WS_CHILDWINDOW | WS_VISIBLE,
			window_info.rect.left, window_info.rect.top, window_info.rect.right - window_info.rect.left, window_info.rect.bottom - window_info.rect.top,
			window_info.hWndParent, 0, 0, 0);

		//Set an acceptable font.
		SendMessage(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(default_font), 0);

		SetWindowLong(hWnd, GWL_ID, window_info.id);
		//SetWindowLong(hWnd, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON);
	}

	//loop_protect_flag = false;

	return hWnd;
}

//Returns the handle of the window created.
Transfer<WINDOW_INFO, HWND>& create_control() {
	std::function<HWND(WINDOW_INFO)> _f(create_control_helper);

	return ___map_impl(_f);
}

Transfer<Nothing, HWND>& create_control2(char class_name[128], RECT rect, LPCSTR text, int id, HWND hWndParent) {
	WINDOW_INFO wi;

	memcpy(wi.class_name, class_name, sizeof(class_name));
	wi.rect = rect;
	wi.text = text;
	wi.id = id;
	wi.hWndParent = hWndParent;

	std::function<WINDOW_INFO(Nothing)> _f(std::bind(const__<WINDOW_INFO, Nothing>, wi, _1));

	return ___map_impl(_f) >> create_control();
}

//////////////////////////////////////

//This is the interface to this module's hook mechanism.
void set_window_hook(HWND hWnd, const std::function<void(const CWPSTRUCT*)>& f) {
	window_hook_mp.insert(std::make_pair(hWnd, f)); // %
}

/////////////////////////////////////

Schedule* retrieve_schedule(HWND hWnd) {
	auto it = window_schedule_mp.find(hWnd);

	if (it == window_schedule_mp.end()) return 0;
	
	
	return &it->second;
}

#endif
