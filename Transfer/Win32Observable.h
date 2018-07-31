#ifndef _WIN32_OBSERVABLE
#define _WIN32_OBSERVABLE

#ifdef WIN32

#include "Observable.h"
#include "Win32Transfer.h"

class Win32Observable: public Observable<CWPSTRUCT> {
protected:
	HWND hWnd;
	std::auto_ptr<std::function<void(const CWPSTRUCT&)> > sink;
public:
	Win32Observable(HWND _hWnd);
	virtual ~Win32Observable();
	virtual void install_handler(std::auto_ptr<std::function<void(const CWPSTRUCT&)> > _sink);
};

//Constructs an observable representing messages sent to the window procedure
//of the specified window.
Observable<CWPSTRUCT>& win32_observable(HWND hwnd);

#endif

#endif