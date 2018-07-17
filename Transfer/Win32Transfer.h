#ifndef _WIN32_SOURCE
#define _WIN32_SOURCE

//Enable on Win32 only.
#ifdef WIN32

#include <windows.h>
#include <tchar.h>
#include <string>
#include <utility>

//It constructs a transfer that retrieves messages from the queue of messages coming-
//into a Windows application. It also discharges the duties of a message loop, dispatching
//messages. Note: this type of transfer blocks the caller, and therefore can be
//used fruitfully *only once* in a transfer network.
Transfer<Nothing, MSG>& win32_source();

//Miscellaneous helpers for processing messages.
Transfer<MSG, MSG>& filter_hwnd(HWND hwnd);
Transfer<MSG, MSG>& filter_code(int code);
Transfer<MSG, LRESULT>& send_message();
Transfer<MSG, BOOL>& post_message();
Transfer<string, BOOL>& set_window_text(HWND hwnd);

#endif

#endif