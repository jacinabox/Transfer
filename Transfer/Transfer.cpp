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

int _plus(int n, int m) { return n + m; }

bool flip_flop(bool x, MSG msg) {
	return !x;
}

std::basic_string<char> pickMessage(bool x) {
	return std::string(x ? "One" : "Two");
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
	//set_window_is_dialog(hwnd);
	//In this example, incoming window messages drive a flip-flop, which changes the title bar text.
	//There is also a drawing test showing some demonstration of a drawing mini-DSL.
	RECT rect{ 100, 100, 400, 400 };
	RECT rect2{ 80, 80, 380, 380 };
	RECT rect3{ 0, 0, 640, 480 };
	POINT pt{ 80, 380 };
	POINT pt2{ 380 , 80 };
	//WINDOW_INFO window_info{ "BUTTON", { 10,10, 200,40}, "Test", 1, hwnd };
	std::function<RECT(MSG)> _f_a(std::bind(const__<RECT, MSG>, rect3, _1));
	//std::function<WINDOW_INFO(MSG)> _f_b(std::bind(const__ < WINDOW_INFO, MSG>, window_info, _1));
	Transfer<Nothing, Nothing>& transfer4 = win32_source() >>
		(filter_code(WM_LBUTTONDOWN) >>
			scanning(true, flip_flop) >>
			map(pickMessage) >>
			set_window_text(hwnd) >>
			map(null_sink2<BOOL>)

			//Vertical pipes (for disjunctive composition) visually indicate the various separate
			//responsibilities of a transfer network, and are highly compositional.
			| map(_f_a) >> 
			resize_window2(hwnd)

			//Test to create controls.
			| map(null_sink2<MSG>) >>
			create_control2("BUTTON", { 10,10,200,40 }, "Test", 100, hwnd) >>
			map(null_sink2<HWND>) >>
			create_control2("BUTTON", { 10,50,200,80 }, "Test 2", 102, hwnd) >>
			map(null_sink2<HWND>)

			| handle_wm_paint2(/*fill_rect(&rect3, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)))
				>> */fill_rect(&rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)))
				>> fill_rect(&rect2, static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH)))
				>> move_to(&pt)
				>> line_to(&pt2, static_cast<HPEN>(GetStockObject(WHITE_PEN)))));

	std::auto_ptr<Observable<Nothing> > observable(&(win32_observable(hwnd)
		>> map(null_sink2<CWPSTRUCT>)
		>> map(null_sink2<Nothing>)));

	Transfer<Nothing, Nothing>::transduce_loop(f0,
		std::auto_ptr<Transfer<Nothing, Nothing> >(&transfer4),
		std::function<void(const Nothing&)>(null_sink<Nothing>));
    return 0;
}