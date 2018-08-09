#ifndef _LAZY
#define _LAZY

#include <functional>
#include <memory>

template<class I> I identity__(I x) {
	return x;
}

//A transfer wrapper, where the internal transfer's construction is deferred until
//processing of the first input. In contrast to the inherent tendency of C++
//towards early binding, this is an example of late-binding. Late binding is
//accomplished by receiving a transfer constructor as a functional, instead
//of a pointer to the transfer itself as usual.
template<class I, class O> class LazyTransfer : public Transfer<I, O> {
protected:
	std::function<std::auto_ptr<Transfer<I, O> >()> tf;
public:
	LazyTransfer(std::function<std::auto_ptr<Transfer<I, O> >()>& _tf)  :tf(_tf) {
	}
	virtual ~LazyTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input, std::function<void(const O&)> &sink) {
		//Construct the transfer.
		std::auto_ptr<Transfer<I, O> > transfer(tf());


		std::auto_ptr<Transfer<I, O> > transfer2 = transfer->transduce(input, sink);


		return transfer2;
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<I, O>* clone() const {
		return new LazyTransfer<I, O> (tf);
	}
	virtual void print_tr_structure() const {
		std::cout << "lazy(_)";
	}
};

/////////////////////////////////////

template<class I, class O> Transfer<I, O>& lazy(std::function<std::auto_ptr<Transfer<I, O> >()>& _tf) {
	return *new LazyTransfer<I, O>(_tf);
}

#endif
