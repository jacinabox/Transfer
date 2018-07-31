#ifndef _Observable
#define _Observable

#include <functional>
#include <memory>
#include <assert.h>

/* The "Transfer" suite of modules is designed to be called in a synchronous event loop.
The Observable class, by contrast, is this library's answer to the problem of /abstraction
inversion/.

If Transfers are arrows, then Observables are contravariant functors.

While both Transfer's "transduce" and Observable's "install_handler" methods receive a functional
as an argument, the implied lifetimes of these arguments are different. The argument to
"transduce" is scoped only to the method call, whilst the argument to "install_handler" is
scoped to the lifetime of the Observable object. The argument to "install_handler" may
(and in common use cases, must) be saved for use as a callback.

As a design tradeoff, observables, unlike Transfers, do not reconstruct themselves, and
so cannot support memory compact-switches or recursive structures. This tradeoff is necessary
because Observable objects need stable addresses, in order to be callable by the OS and
so forth.

Thread safety:

The calls to the handlers for all observable objects must be synchronous. This restriction
may be relaxed in the future.
*/
template<class T> class Observable {
public:
	Observable() {
	}
	
	virtual ~Observable() {
	}
	/*Installing a handler causes any handler that was already there to be replaced.*/
	virtual void install_handler(std::auto_ptr<std::function<void(const T&)> > _sink) {
	}
};

template<class T> class MergeObservable : public Observable<T> {
protected:
	std::auto_ptr<Observable<T> > p1, p2;
public:
	MergeObservable(std::auto_ptr<Observable<T> > _p1, std::auto_ptr<Observable<T> > _p2) : p1(_p1), p2(_p2) {
		assert(p1.get());
		assert(p2.get());
	}
	virtual ~MergeObservable() {
	}
	virtual void install_handler(std::auto_ptr<std::function<void(const T&)> > _sink) {

		p1->install_handler(_sink);
		p2->install_handler(_sink);
	}
};

template<class T> Observable<T>& operator |(Observable<T>& Observable1, Observable<T>& Observable2) {
	return *new MergeObservable<T>(std::auto_ptr<Observable<T> >(&Observable1),
		std::auto_ptr<Observable<T> >(&Observable2));
}

//////////////////////////////////////////////

template<class I, class O> void extend_Observable_helper(std::auto_ptr<Transfer<I, O> >* p_transfer,
	std::auto_ptr<std::function<void(const O&)> > _sink,
	const I& input) {
	ptr_assignment_helper(*p_transfer, *p_transfer, (*p_transfer)->transduce(sink, input));
}

template<class I, class O> class ExtendObservable : public Observable<O> {
protected:
	std::auto_ptr<Observable<I> > p_Observable;
	mutable std::auto_ptr<Transfer<I, O> > p_transfer;
public:
	ExtendObservable(std::auto_ptr<Observable<I> > _p_Observable, std::auto_ptr<Transfer<I, O> > _p_transfer) :
		p_Observable(_p_Observable), p_transfer(_p_transfer) {
		assert(p_Observable.get());
		assert(p_transfer.get());
	}
	virtual ~ExtendObservable() {
	}
	virtual void install_handler(std::auto_ptr<std::function<void(const O&)> > _sink) {
		std::function<void(const O&)> sink2(std::bind(extend_Observable_helper<I, O>, p_transfer, _sink, _1));

		p_Observable->install_handler(sink2);
	}
};

/* As a contravariant functor, Observables enjoy post-composition of an arrow action, i.e. Transfers.*/
template<class I, class O> Observable<O>& extend(Observable<I>& Observable, Transfer<I, O>& transfer) {
	return *new ExtendTransfer<I, O>(std::auto_ptr<Observable<O> >(&Observable),
		std::auto_ptr<Transfer<I, O> >(&transfer));
}

////////////////////////////////////////////

#endif