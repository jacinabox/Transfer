#ifndef _MISC
#define _MISC

#include <list>
#include <functional>
#include <memory>
#include <iostream>

using namespace std::placeholders;

template<class I, class I_PREDICATE> std::list<I> splitting_helper(I_PREDICATE& predicate, const std::list<I> & ls, I x) {
	std::list<I> ls2;

	if (!(ls.empty() || predicate(ls.back()))) { const_cast<std::list<I>& >(ls).swap(ls2); }
	ls2.push_back(x);
	return ls2;
}

template<class I, class I_PREDICATE> bool splitting_helper2(I_PREDICATE& predicate, const std::list<I>& ls) {
	return ls.empty() || predicate(ls.back());
}

//Note: Predicates have to accept their arguments passed by value; otherwise we are creating a list with reference
//value type and that's not allowed.
template<class I_PREDICATE> Transfer<typename I_PREDICATE::argument_type, std::list<typename I_PREDICATE::argument_type> >&
	_splitting(I_PREDICATE& predicate) {

	std::function<std::list<typename I_PREDICATE::argument_type>(const std::list<typename I_PREDICATE::argument_type>&,
		I_PREDICATE::argument_type)> f1(
		std::bind(splitting_helper<typename I_PREDICATE::argument_type, I_PREDICATE>, predicate, _1, _2));
	std::function<bool(std::list<typename I_PREDICATE::argument_type>)> f2(
		std::bind(splitting_helper2<typename I_PREDICATE::argument_type, I_PREDICATE>, predicate, _1));
	std::list<typename I_PREDICATE::argument_type> init;

	return scanning(init, f1)
		>>
		filter(f2);
}

#define splitting(PREDICATE) (_splitting(make_function(PREDICATE)))

template<class X> void cout_list(const std::list<X>& ls) {
	std::list<X>& ls2 = const_cast<std::list<X>&>(ls);
	std::list<X>::iterator it;

	it = ls2.begin();
	if (it == ls2.end())
	{
		cout << endl;
	}
	else
	{
		std::cout << *it++;
		for (;it != ls2.end();++it) {
			std::cout << ", " << *it;
		}
		cout << endl;
	}
}

//The default implementation of Transfer passes on no messages. This is just an alias.
template<class I, class O> Transfer<I, O>& empty() {
	return *new Transfer<I, O>();
}

#endif