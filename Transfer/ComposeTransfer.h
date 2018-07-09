#ifndef _COMPOSE
#define _COMPOSE

//#include <utility>
#include <functional>
#include <memory>
#include <assert.h>

using namespace std::placeholders;

template<class O1, class O2> void helper(std::auto_ptr<Transfer<O1, O2> >* new_transfer,
	std::function<void(const O2&)>& sink,
	const O1& o1) {
	*new_transfer = (*new_transfer)->transduce(o1, sink);
	
	
}

/*template<class O1, class O2, class SINK_O2> class Helper_function_type(Transfer<O1, O2, SINK_O2>& transfer,
	SINK_O2& sink,
	std::auto_ptr<Transfer<O1, O2, SINK_O2> >& new_transfer,
	O1& o1);

template<class O1, class O2, class SINK_O2> typedef std::_Binder<void,
	Helper_function_type<O1, O2, SINK_O2>,
	Transfer<O1, O2, SINK_O2>&,
	SINK_O2&,
	std::auto_ptr<Transfer<O1, O2, SINK_O2> > >
	Helper_binder;*/

template<class I, class O1, class O2> class ComposeTransfer : public Transfer<I, O2> {
protected:
	std::auto_ptr<Transfer<I, O1> > transfer1;
	mutable std::auto_ptr<Transfer<O1, O2> > transfer2;


public:

	ComposeTransfer(std::auto_ptr<Transfer<I, O1 > >& _transfer1,
		std::auto_ptr<Transfer<O1, O2> >& _transfer2)
		: transfer1(_transfer1), transfer2(_transfer2)
	{
		assert(transfer1.get());
		assert(transfer2.get());
	}
	
	virtual ~ComposeTransfer() {
	}

	virtual std::auto_ptr<Transfer<I, O2> > transduce(const I& input,
		std::function<void(const O2&)>& sink) const {


		//Set up variables to capture the components that will make up the succeeding transfer.
		std::auto_ptr<Transfer<I, O1> > new_transfer1;
		
		
		//Set up a function to serve as sink for the first transfer. The second component
		//of the succeeding transfer will be received through the new_transfer2 variable.

		


		std::function<void(const O1&)> f(std::bind(helper<O1, O2>, &transfer2, sink, _1));

		//Process the input.
		new_transfer1 = transfer1->transduce(input, f);


		//Reconstruct a succeeding transfer.
		return std::auto_ptr<Transfer<I, O2> >(new ComposeTransfer(new_transfer1, transfer2));
	}
};

/////////////////////////////////////

template<class I, class O1, class O2> Transfer<I, O2>*
	operator >>(Transfer<I, O1>& _transfer1,
	Transfer<O1, O2>* _transfer2) {

	//Constructor wrapper.

	return new ComposeTransfer<I, O1, O2>(std::auto_ptr<Transfer<I, O1> >(&_transfer1), 
		std::auto_ptr<Transfer<O1, O2> >(_transfer2));
}

#endif