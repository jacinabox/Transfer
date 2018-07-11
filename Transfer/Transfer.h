#ifndef _TRANSFER
#define _TRANSFER

// #include <utility>
// #include <list>
#include <functional>
#include <memory>
#include <assert.h>
//#include <iostream>

typedef int Nothing;

template<class I, class O> class Transfer {
public:
	Transfer() {
	}

	virtual ~Transfer() {
	}

	//A transfer in general cannot modify itself (because transduce is const), so it has
	//to prepare a representation with its new state. Places where a transfer object
	//absolutely has to modify its state internally, will end up being marked mutable.
	//Transfers may be modified to take ownership of their components, and those components
	//set to null.
	//
	//The caller must take ownership of the transfer pointer returned, except when the
	//pointer returned is null. When the pointer returned is null, this indicates that
	//the calling object can continue to use the transfer object it already has, and
	//just called.
	//
	//If a transfer object is non-stateless (as indicated by the result of is_stateless
	//method), then it may return a stateless object from transduce. If it is stateless,
	//transduce must return a null pointer.
	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input,
		std::function<void(const O&)>& sink) const {
		return std::auto_ptr<Transfer<I, O> >(0);
	}

	//Indicates whether the transfer type returns a null pointer or whether it
	//reconstructs itself. This information is used to optimize.
	virtual bool is_stateless() const {
		return true;
	}

	//The clone method must implement copy semantics.
	virtual Transfer<I, O>* clone() const {
		return new Transfer<I, O>();
	}

	static void transduce_loop(std::function<I()>& generator,
		std::auto_ptr<Transfer<I, O> > transfer,
		std::function<void(const O&)>& sink) {
		while (true) {
			I input = generator();
			ptr_assignment_helper(transfer, transfer, transfer->transduce(input, sink));
		}
	}
};
/*




template<class I> class SinkTransfer : public Transfer<I, Nothing> {
protected:
	std::function<void(const I&)> f;
public:
	SinkTransfer(std::function<void(const I&)> _f) : f(_f) {
	}
	virtual ~SinkTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, Nothing> > transduce(const I& input, Transfer<Nothing, Nothing>& sink) const {
		f(input);
		return auto_ptr<Transfer<I, Nothing> >(0);
	}
};*/


/*template<class I> void const_f(I x) {
}

template<class I> void feed_sink(Transfer<I, Nothing>& sink, const I& input) {

	function<void(const Nothing&)> f(static_cast<void(*)(const Nothing&)> (const_f));
	SinkTransfer<Nothing> sinkTransfer(f);

	sink.transduce(input, sinkTransfer);
}*/

//An example of a simple transfer.

class ExampleTransfer : public Transfer<int, int> {
public:
	virtual ~ExampleTransfer() {
		//std::cout << "Ah yes, much destructor, very memory efficient" << std::endl;
	}

	virtual std::auto_ptr<Transfer<int, int> > transduce(const int& input,
		std::function<void(const int&)>& sink) const {
		sink( 1 + input);
		return std::auto_ptr<Transfer<int, int> >(0);
	}
};

////////////////////////////////////

template<class I, class O> void ptr_assignment_helper(std::auto_ptr<Transfer<I, O> >& ptr, std::auto_ptr<Transfer<I, O> >& ptr2, std::auto_ptr<Transfer<I, O> > ptr3) {
	ptr.reset(ptr3.get() ? ptr3.release() : ptr2->clone());
}

#endif