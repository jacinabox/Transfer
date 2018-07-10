#ifndef _MISC
#define _MISC

#include <list>
#include <functional>
#include <memory>
#include <iostream>

using namespace std::placeholders;

template<class I, class I_PREDICATE> std::list<I> splitting_helper(I_PREDICATE& predicate, const std::list<I> & ls, const I& x) {
	std::list<I> ls2;

	if (!(ls.empty() || predicate(ls.back()))) { const_cast<std::list<I>& >(ls).swap(ls2); }
	ls2.push_back(x);
	return ls2;
}

template<class I, class I_PREDICATE> bool splitting_helper2(I_PREDICATE& predicate, const std::list<I>& ls) {
	return ls.empty() || predicate(ls.back());
}

template<class I, class I_PREDICATE> Transfer<I, std::list<I> >*
	splitting(const I& type_determiner,I_PREDICATE& predicate) {
	std::list<I> type_determiner2;
	std::function<std::list<I>(const std::list<I>&, const I&)> f1(
		std::bind(splitting_helper<I, I_PREDICATE>, predicate, _1, _2));
	std::function<bool(const std::list<I>&)> f2(
		std::bind(splitting_helper2<I, I_PREDICATE>, predicate, _1));
	std::list<I> init;

	return *scanning(type_determiner, init, f1)
		>>
		filter(type_determiner2, f2);
}

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

template<class I, class O, class FUNCTIONAL> Transfer<I, O>* map(const I& type_determiner, const O& type_determiner2, FUNCTIONAL& f) {
	std::function<O(const O&, const I&)> f2(std::bind(f, _2));

	return scanning(type_determiner, type_determiner2, f2, true);
}

#endif