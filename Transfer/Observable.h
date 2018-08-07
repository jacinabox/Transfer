#ifndef _Observable
#define _Observable

#include <functional>
#include <memory>
#include <assert.h>

/* The "Transfer" suite of modules is designed to be called in a synchronous event loop.
The Observable class, by contrast, is this library's answer to the problem of /abstraction
inversion/. The usual reason to employ abstraction inversion, is that it gets too unwieldy to process
some type of message in the main reactimate loop. In that case, abstraction inversion
is employed to feed the message stream in some ad hoc manner, and the reactimate loop is kept
clear of such concern.

Hughes arrows are a model of computation which encode input types to a procedure explicitly.
Transfers are an exemplar. This Observable class defines a co-variant functor, to which
arrows precompose.

While both Transfer's "transduce" and Observable's "install_handler" methods receive a functional
as an argument, the distinction lies in the lifetime of such an argument; the transduce method cannot
save it, while the Observable can (and in common cases, must).

As a design tradeoff, observables, unlike Transfers, do not prepare more memory compact copies
of themselves as they go, and so cannot support memory compact-switches or recursive structures.
This tradeoff is necessary because Observable objects need stable addresses, in order to be callable
by the OS.

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
	virtual void install_handler(std::function<void(const T&)> _sink) {
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
	virtual void install_handler(std::function<void(const T&)> _sink) {



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
	std::function<void(const O&)> _sink,
	const I& input) {
	ptr_assignment_helper(*p_transfer, *p_transfer, (*p_transfer)->transduce(input, _sink));
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
	virtual void install_handler(std::function<void(const O&)> _sink) {
		std::function<void(const I&)> sink2(std::bind(extend_Observable_helper<I, O>, &p_transfer, _sink, _1));


		p_Observable->install_handler(sink2);

	}
};

/* As a co-variant functor, Observables enjoy pre-composition of an arrow action.*/
template<class I, class O> Observable<O>& operator >>(Observable<I>& observable, Transfer<I, O>& transfer) {
	return *new ExtendObservable<I, O>(std::auto_ptr<Observable<I> >(&observable),
		std::auto_ptr<Transfer<I, O> >(&transfer));
}

////////////////////////////////////////////

#endif
