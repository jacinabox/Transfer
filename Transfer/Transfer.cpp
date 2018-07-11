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

template<class T, class T2> T const__(T& x, const T2& x2) {
	return x;
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

//I have to make these interfaces work with all reasonable permutations of references,
//const qualifiers.
int main()
{
	//function<int(int, int)> f(*_plus);
	char n = 0;

	function<char()> f0(std::bind(identity__<char>, n));
	
	function<bool(char)> f(predicate);

	//The transfer interface uses operator overloading; please see the code of splitting in
	//Miscellaneous.h for example.
	auto_ptr<Transfer<char, list<char> > > transfer(&
			(map(incr) >> splitting(f))
		);

	function<char()> f2(reader);
	function<void(const list<char>&)> _printer(printer);

	// The example case accepts characters from stdin, splits them by spaces, and prints them.
	Transfer<char, list<char> >::transduce_loop(f2, transfer, _printer);
    return 0;
}