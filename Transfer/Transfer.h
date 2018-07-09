#ifndef _TRANSFER
#define _TRANSFER

// #include <utility>
// #include <list>
#include <functional>
#include <memory>
//#include <iostream>

typedef int Nothing;

template<class I, class O> class Transfer {
public:
	virtual ~Transfer() {
	}

	//A transfer effectively cannot modify itself (because transduce is const), so it has
	//to prepare a representation with its new state. Places where a transfer object
	//absolutely has to modify its state internally, will end up being marked mutable.
	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input,
		std::function<void(const O&)>& sink) const {
		return std::auto_ptr<Transfer<I, O> >(0);
	}

	static void transduce_loop(std::function<I()>& generator,
		std::auto_ptr<Transfer<I, O> > transfer,
		std::function<void(const O&)>& sink) {
		while (true) {
			I input = generator();
			transfer = transfer->transduce(input, sink);
		}
	}
};
/*
//When transfers appear in their capacity as sinks (telltale is a Nothing
//output) they are not expected to behave as transfers usually behave,
//return sensible successor transfers, etc.

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
		return std::auto_ptr<Transfer<int, int> >(new ExampleTransfer());
	}
};

#endif