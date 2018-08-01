#include "stdafx.h"
#include <functional>

#ifdef WIN32

void win32_observable_adapter(std::function<void(const CWPSTRUCT&)>& _f, const CWPSTRUCT* input) {
	_f(*input);
}

Win32Observable::Win32Observable(HWND _hWnd) : hWnd(_hWnd) {
}

Win32Observable::~Win32Observable() {
}

void Win32Observable::install_handler(std::function<void(const CWPSTRUCT&)> _sink) {
	sink = _sink;

	std::function<void(const CWPSTRUCT*)> _f(std::bind(win32_observable_adapter, sink, _1));

	set_window_hook(hWnd, _f);
}

Observable<CWPSTRUCT>& win32_observable(HWND hwnd) {
	return *new Win32Observable(hwnd);
}

#endif