// Transfer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <functional>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <conio.h>

/*const int& _identity(const int& x) {
	return x;
}*/

char reader() {
	return getch();
}

void printer0(const int& n) {
	cout<< (char)n;
}

void printer(const list<char>& ls) {
	cout_list(ls);
}

bool predicate(char x) {
	return x == 33; //Test for space character.
}

char incr(char c) { return c + 1; }

template<typename I, typename O> auto_ptr<Transfer<I, O> > make_auto_ptr
(Transfer<I, O>* x) {

	//Auto ptr constructor wrapper.

	return auto_ptr<Transfer<I, O> >(x);
}

template<class X > Transfer<X, X>& identity_() {

	//Identity wrapper.

	return *new IdentityTransfer<X>();
}

int _plus(int n, int m) { return n + m; }

bool flip_flop(bool x, MSG msg) {
	return !x;
}

LPTSTR pickMessage(bool x) {
	return (x ? _T("One") : _T("Two"));
}

//I have to make these interfaces work with all reasonable permutations of references,
//const qualifiers.
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//function<int(int, int)> f(*_plus);
	char n = 0;
	list<char> ls;

	function<Nothing()> f0(std::bind(identity__<Nothing>, Nothing()));
	
	function<bool(char)> f(predicate);

	//The transfer interface uses operator overloading; please see the code of splitting in
	//Miscellaneous.h for example.
	auto_ptr<Transfer<char, list<char> > > transfer(&
			(map(incr) >> splitting(f))
		);
	std::function<std::auto_ptr<Transfer<char, list<char> > >(char)> _f;
	auto_ptr<Transfer<char, list<char> > > transfer2(&r_switch<char, list<char> >(_f, *transfer));
	auto_ptr<Transfer<char, char> > transfer3(&feedback(map(incr)));

	function<char()> f2(reader);
	function<void(const list<char>&)> _printer(printer);

	// The example case accepts characters from stdin, splits them by spaces, and prints them.
	// Note: enable to see an example of processing with transfers in console mode.
	//Transfer<char, list<char> >::transduce_loop(f2, transfer, _printer);

	/////////////////////////////////////////

	HWND hwnd = create_frame_window(_T("Test"), NULL, NULL);

	//In this example, incoming window messages drive a flip-flop, which changes the title bar text.
	//There is also a drawing test showing some demonstration of a drawing mini-DSL.
	RECT rect{ 100, 100, 400, 400 };
	RECT rect2{ 80, 80, 380, 380 };
	Transfer<Nothing, Nothing>& transfer4 = win32_source() >>
		(scanning(true, flip_flop) >>
			map(pickMessage) >>
			set_window_text(hwnd) >>
			map(null_sink2<BOOL>)
			/*| handle_wm_paint(fill_rect(&rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)))
				>> fill_rect(&rect2, static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH))))*/);

	Transfer<Nothing, Nothing>::transduce_loop(f0,
		std::auto_ptr<Transfer<Nothing, Nothing> >(&transfer4),
		std::function<void(const Nothing&)>(null_sink<Nothing>));
    return 0;
}