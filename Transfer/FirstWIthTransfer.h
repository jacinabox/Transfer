#ifndef _FIRST_WITH
#define _FIRST_WITH

//The behaviour of this transfer can also be implemented by a scan. This implementation
//has the advantage that it can completely free up its representation after one
//input step, making it suitable for recursive transformer definitions.
template<class I, class O> class FirstWithTransfer : public Transfer<I, O> {
protected:
	std::auto_ptr<Transfer<I, O> > transfer1;
	mutable std::auto_ptr<Transfer<I, O> > transfer2;

public:
	FirstWithTransfer(std::auto_ptr<Transfer<I, O> > _transfer1, std::auto_ptr<Transfer<I, O> > _transfer2)
		: transfer1(_transfer1), transfer2(_transfer2) {
	}

	virtual ~FirstWithTransfer() {
	}

	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input, std::function<void(const O&)>& sink) const {


		//Process input with transfer1 for one step.
		transfer1->transduce(input, sink);
		


		//Then continue as the second transfer.
		return transfer2;
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<I, O>* clone() const {
		return new FirstWithTransfer(std::auto_ptr<Transfer<I, O> >(transfer1->clone()), transfer2);
	}
};

///////////////////////////////////////

template<class I, class O> Transfer<I, O>& first_with(Transfer<I, O>& transfer1, Transfer<I, O>& transfer2) {
	return *new FirstWithTransfer<I, O>(std::auto_ptr<Transfer<I, O> >(&transfer1),
		std::auto_ptr<Transfer<I, O> >(&transfer2));
}

#endif