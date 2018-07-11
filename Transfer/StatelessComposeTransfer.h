#ifndef _STATELESS_COMPOSE
#define _STATELESS_COMPOSE

#include <memory>
#include <functional>
#include <assert.h>

template<class I, class O1, class O2> class StatelessComposeTransfer : public Transfer<I, O2> {
protected:
	std::auto_ptr<Transfer<I, O1> > transfer1;
	mutable std::auto_ptr<Transfer<O1, O2> > transfer2;

public:

	StatelessComposeTransfer(std::auto_ptr<Transfer<I, O1 > >& _transfer1,
		std::auto_ptr<Transfer<O1, O2> >& _transfer2)
		: transfer1(_transfer1), transfer2(_transfer2)
	{
		assert(transfer1.get());
		assert(transfer2.get());
	}
	
	virtual ~StatelessComposeTransfer() {
	}

	virtual std::auto_ptr<Transfer<I, O2> > transduce(const I& input,
		//A stateless version of ComposeTransfer.
		std::function<void(const O2&)>& sink) const {


		
		
		
		
		
		std::function<void(const O1&)> f(std::bind(helper<O1, O2>, &transfer2, sink, _1));

		
		transfer1->transduce(input, f);


		return std::auto_ptr<Transfer<I, O2> >(0);
	}

	//StatelessComposeTransfers are always assumed to be stateless because I
	//only construct transfers using it, when the component transfers are
	//known to be stateless.
	virtual Transfer<I, O2>* clone() const {
		/*Transfer<O1, O2>* transfer2_ = transfer2.release();


		transfer2.reset(new Transfer<O1, O2>());*/

		return new StatelessComposeTransfer<I, O1, O2>(std::auto_ptr<Transfer<I, O1> >(transfer1->clone()),
			std::auto_ptr<Transfer<O1, O2> >(transfer2->clone()));
	}
};


#endif