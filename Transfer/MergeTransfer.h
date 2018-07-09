#ifndef _MERGE
#define _MERGE

#include <functional>
#include <memory>

template<class I, class O> class MergeTransfer : public Transfer<I, O> {
protected:
	std::auto_ptr<Transfer<I, O> > transfer1, transfer2;

public:
	MergeTransfer(std::auto_ptr<Transfer<I, O> >& _transfer1,
		std::auto_ptr<Transfer<I, O> >& _transfer2) :
		transfer1(_transfer1), transfer2(_transfer2) {
		assert(transfer1.get());
		assert(transfer2.get());
	}
	virtual ~MergeTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input, std::function<void(const O&)>& sink) const {
		//Split the input stream and feed it through both transfers.
		std::auto_ptr<Transfer<I, O> > new_transfer1 = transfer1->transduce(input, sink);
		std::auto_ptr<Transfer<I, O> > new_transfer2 = transfer2->transduce(input, sink);

		//Reconstruct a new transfer to use for the next input.
		return std::auto_ptr<Transfer<I, O> >( new MergeTransfer<I, O>(new_transfer1, new_transfer2));
	}
};

////////////////////////////////

template<class I, class O> Transfer<I, O>* operator |(Transfer<I, O>& transfer1, Transfer<I, O>* transfer2) {
	return new MergeTransfer<I, O> (std::auto_ptr<Transfer<I, O> >(&transfer1),
		std::auto_ptr<Transfer<I, O> >(transfer2));
}

#endif