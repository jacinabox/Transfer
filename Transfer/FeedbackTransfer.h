#ifndef _FEEDBACK
#define _FEEDBACK

#include <list>
#include <iterator>
#include <memory>
#include <functional>
#include <algorithm>

template<class X> void feedback_helper(std::function<void(const X&)>& sink,
	std::list<X>* ls,
	const X& input) {
	ls->push_back(input);
	sink(input);
}

//Note: use of a feedback mechanism without care can lead to infinite loops.
template<class X> class FeedbackTransfer : public Transfer<X, X> {
protected:
	std::auto_ptr<Transfer<X, X> > transfer;
public:
	FeedbackTransfer(std::auto_ptr<Transfer<X, X> > _transfer) :transfer(_transfer) {
	}
	virtual ~FeedbackTransfer() {
	}
	virtual std::auto_ptr<Transfer<X, X> > transduce(const X& input, std::function<void(const X&)>& sink)
		{
		std::list<X> ls(1, input);

		X x;
		std::function<void(const X&)> f(std::bind(feedback_helper<X>, sink, &ls, _1));

		while (!ls.empty()) {
			x = ls.front();
			ls.pop_front();
			ptr_assignment_helper(transfer, transfer, transfer->transduce(x, f));



		}
		return std::auto_ptr<Transfer<X, X> >(new FeedbackTransfer(transfer));
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<X, X>* clone() const {
		return new FeedbackTransfer(std::auto_ptr<Transfer<X, X> >(transfer->clone()));
	}
	virtual void print_tr_structure() const {
		std::cout << "feedback(";
		transfer->print_tr_structure();
		std::cout << ")";
	}

};

/////////////////////////////////////

template<class X> Transfer<X, X>& feedback(Transfer<X, X>& transfer) {
	return *new FeedbackTransfer<X>(std::auto_ptr<Transfer<X, X> >(&transfer));
}

#endif
