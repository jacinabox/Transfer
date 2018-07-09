#include "stdafx.h"
#include <utility>
#include <functional>

using namespace std::placeholders;

template<class O1, class O2, class SINK_O2> class BespokeBinder {
protected:
	Transfer<O1, O2, SINK_O2>& transfer;
	SINK_O2& sink;
	std::auto_ptr<Transfer<O1, O2, SINK_O2> >& new_transfer;

public:
	BespokeBinder(Transfer<O1, O2, SINK_O2>& _transfer,
	SINK_O2& _sink,
	std::auto_ptr<Transfer<O1, O2, SINK_O2> >& _new_transfer)
		: transfer(_transfer),
		sink(_sink),
		new_transfer(_new_transfer) {
	}

	void operator() (const O1& o1) {
		new_transfer = transfer.transduce(o1, sink);
	}
};

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

template<class I, class O1, class O2, class SINK_O2> class ComposeTransfer : public Transfer<I, O2, SINK_O2> {
protected:
	std::auto_ptr<Transfer<I, O1, BespokeBinder<O1, O2, SINK_O2> > > transfer1;
	std::auto_ptr<Transfer<O1, O2, SINK_O2> > transfer2;

public:

	ComposeTransfer(std::auto_ptr<Transfer<I, O1, BespokeBinder<O1, O2, SINK_O2> > > _transfer1,
		std::auto_ptr<Transfer<O1, O2, SINK_O2> > _transfer2)
	{
		transfer1 = _transfer1;
		transfer2 = _transfer2;
	}
	
	virtual ~ComposeTransfer() {
	}

	virtual std::auto_ptr<Transfer<I, O2, SINK_O2> > transduce(const I& input,
		SINK_O2& sink) const {


		//Set up variables to capture the components that will make up the succeeding transfer.
		auto_ptr<Transfer<I, O1, BespokeBinder<O1, O2, SINK_O2> > > new_transfer1;
		auto_ptr<Transfer<O1, O2, SINK_O2> > new_transfer2;
		//Set up a function to serve as sink for the first transfer. The second component
		//of the succeeding transfer will be received through the new_transfer2 variable.

		BespokeBinder<O1, O2, SINK_O2> f(*transfer2, sink, new_transfer2);

		//Process the input.
		new_transfer1 = transfer1->transduce(input, f);


		//Reconstruct a succeeding transfer.
		return auto_ptr<Transfer<I, O2, SINK_O2> >(new ComposeTransfer(new_transfer1, new_transfer2));
	}
};

/////////////////////////////////////
