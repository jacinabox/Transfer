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
	ptr_assignment_helper(*new_transfer, *new_transfer, (*new_transfer)->transduce(o1, sink));
	
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
	//Mutation of the transfer1 variable, is limited to taking ownership.
	std::auto_ptr<Transfer<I, O1> > transfer1;
	std::auto_ptr<Transfer<O1, O2> > transfer2;


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
		std::function<void(const O2&)>& sink) {


		//Set up variables to capture the components that will make up the succeeding transfer.
		std::auto_ptr<Transfer<I, O1> > new_transfer1;
		
		
		//Set up a function to serve as sink for the first transfer. The second component
		//of the succeeding transfer will be received through the new_transfer2 variable.

		


		std::function<void(const O1&)> f(std::bind(helper<O1, O2>, &transfer2, sink, _1));

		//Process the input.
		ptr_assignment_helper(new_transfer1, transfer1, transfer1->transduce(input, f));
		


		//Reconstruct a succeeding transfer.
		return std::auto_ptr<Transfer<I, O2> >(new ComposeTransfer(new_transfer1, transfer2));
	}

	virtual bool is_stateless() const {
		return false;
	}

	virtual void print_tr_structure() const {
		std::cout << "(";
		transfer1->print_tr_structure();
		std::cout << ") >> (";
		transfer2->print_tr_structure();
		std::cout << ")";
	}

	virtual Transfer<I, O2>* clone() const {
		return new ComposeTransfer<I, O1, O2>(std::auto_ptr<Transfer<I, O1> >(transfer1->clone()),
			std::auto_ptr<Transfer<O1, O2> >(transfer2->clone()));
	}
};

/////////////////////////////////////

template<class I, class O1, class O2> Transfer<I, O2>&
operator >>(Transfer<I, O1>& _transfer1,
	Transfer<O1, O2>& _transfer2) {

	//Constructor wrapper - also selects an appropriate implementation based on whether
	//or not the component transfers are stateless.

	//Let's explicitly rewrite the transfer to elide the identity components....
	if (dynamic_cast<IdentityTransfer<I>*>(&_transfer1)) {
		//If this was Haskell, the pattern match would generate type equality-evidence validating
		//the coercion. This ain't Haskell.
		return dynamic_cast<Transfer<I, O2>&>(_transfer2);
	} else if (dynamic_cast<IdentityTransfer<O1>*>(&_transfer2)) {
		return dynamic_cast<Transfer<I, O2>&>(_transfer1);
	}

	if (_transfer1.is_stateless() && _transfer2.is_stateless()) {
		//Then use the more efficient version.
		return *new StatelessComposeTransfer<I, O1, O2>(std::auto_ptr<Transfer<I, O1> >(&_transfer1),
			std::auto_ptr<Transfer<O1, O2> >(&_transfer2));
	}
	else {
		return *new ComposeTransfer<I, O1, O2>(std::auto_ptr<Transfer<I, O1> >(&_transfer1),
				std::auto_ptr<Transfer<O1, O2> >(&_transfer2));
	}
}

#endif
