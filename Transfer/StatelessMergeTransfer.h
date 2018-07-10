#ifndef _STATELESS_MERGE
#define _STATELESS_MERGE

#include <functional>
#include <memory>

template<class I, class O> class StatelessMergeTransfer : public Transfer<I, O> {
protected:
	mutable std::auto_ptr<Transfer<I, O> > transfer1, transfer2;

public:
	StatelessMergeTransfer(std::auto_ptr<Transfer<I, O> >& _transfer1,
		std::auto_ptr<Transfer<I, O> >& _transfer2) :
		transfer1(_transfer1), transfer2(_transfer2) {
		assert(transfer1.get());
		assert(transfer2.get());
	}
	virtual ~StatelessMergeTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input, std::function<void(const O&)>& sink) const {
	
		transfer1 = transfer1->transduce(input, sink);
		transfer2 = transfer2->transduce(input, sink);

		return std::auto_ptr<Transfer<I, O> >(const_cast<StatelessMergeTransfer<I, O>*>(this));
	}
	virtual Transfer<I, O>* clone() const {
		return new StatelessMergeTransfer<I, O>(std::auto_ptr<Transfer<I, O> >(transfer1->clone()),
			std::auto_ptr<Transfer<I, O> >(transfer2->clone()));
	}
};

#endif
