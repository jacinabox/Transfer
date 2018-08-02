#include "stdafx.h"
#include <windows.h>
#include <functional>
#include <algorithm>

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

	return //(filter_code(WM_WINDOWPOSCHANGED) | filter_code(WM_CREATE)) >>
		filter_hwnd(hWndParent) >>
		map(_f) >>
		resize_window();
}

//static bool _test_flag=false;

class WindowLayoutObject : public LayoutObject {
protected:
	HWND hWnd;
	unsigned padding_pixels;
	mutable SIZE sz = { 0 };
	//FLOAT_TYPE x;
	mutable unsigned id;
public:
	WindowLayoutObject(HWND _hWnd, unsigned _padding_pixels) :
		hWnd(_hWnd), padding_pixels(_padding_pixels), id(0) {
		/*x = (_test_flag ? FLOAT_LEFT : INLINE);
		_test_flag = !_test_flag;*/
	}
	virtual ~WindowLayoutObject() {
	}
	virtual unsigned get_width() const {
		RECT rt;

		//The size is cached.
		if (!sz.cx) {
			GetClientRect(hWnd, &rt);
			sz.cx = rt.right+padding_pixels;
			sz.cy = rt.bottom+padding_pixels;
		}

		return sz.cx;
	}
	virtual unsigned get_height() const {
		get_width();

		return sz.cy;
	}
	virtual FLOAT_TYPE get_float_type() const {
		return INLINE;
	}
	virtual HWND get_window() const {
		return hWnd;
	}
	virtual int get_id() const {
		if (!id) id = GetWindowLong(hWnd, GWL_ID);

		return id;
	}
};

bool window_layout_comparator(const LayoutObject* wlo1, const LayoutObject* wlo2) {
	return dynamic_cast<const WindowLayoutObject*>(wlo1)->get_id() <
		dynamic_cast<const WindowLayoutObject*>(wlo2)->get_id();
}

/*FLOAT_TYPE float_type, */
Nothing size_layout_helper(HWND hWndParent, unsigned padding_pixels, MSG msg) {
	std::vector<Paragraph> vector;
	LeftJustifyingLayoutDelegate ld;
	RECT rt;
	std::vector<LAYOUT_LINE_RESULT>::const_iterator it;
	LO_ITERATOR it2;
//	_test_flag = false;
	vector.resize(1);
	vector.begin()->ld = &ld;
	
	//Construct window layout objects corresponding to the child windows.
	HWND hWnd = GetWindow(hWndParent, GW_CHILD);

	if (!hWnd) return Nothing(); //Nothing to do.
	
	vector.begin()->vector.push_back(new WindowLayoutObject(hWnd, padding_pixels));
	while (hWnd=GetWindow(hWnd, GW_HWNDNEXT)) {
		vector.begin()->vector.push_back(new WindowLayoutObject(hWnd, padding_pixels));
	}
	std::sort(vector.begin()->vector.begin(), vector.begin()->vector.end(), window_layout_comparator);

	//Get width of parent window.
	GetClientRect(hWndParent, &rt);

	LayoutInternalState lis(rt.right - padding_pixels, vector);
	lis.layout(rt.bottom);
	const std::vector<LAYOUT_LINE_RESULT>& result = lis.get_result();

	//Position child windows.
	for (it = result.cbegin();it != result.cend();++it) {
		SetWindowPos(dynamic_cast<const WindowLayoutObject*>(it->lo)->get_window(), 0,
			it->point.x+padding_pixels, it->point.y+padding_pixels, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	//Free up memory.
	for (it2 = vector.begin()->vector.begin();it2 != vector.begin()->vector.end();++it2) {
		delete *it2;
	}

	return Nothing();
}

Transfer<MSG, Nothing>& size_children_according_to_layout(HWND hWndParent, unsigned padding_pixels) {
	std::function<Nothing(MSG)> _f(std::bind(size_layout_helper, hWndParent, padding_pixels, _1));

	return //(filter_code(WM_WINDOWPOSCHANGED) | filter_code(WM_CREATE))>>
		filter_hwnd(hWndParent) >>
		map(_f);
}

#endif