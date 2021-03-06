#ifndef _FILTER
#define _FILTER

#include <functional>
#include <memory>

template<class I, class I_PREDICATE> class FilterTransfer : public Transfer<I, I> {
protected:
	I_PREDICATE predicate;
public:
	FilterTransfer(const I_PREDICATE& _predicate) : predicate(_predicate) {
	}
	virtual ~FilterTransfer() {
		;
	}
	virtual std::auto_ptr<Transfer<I, I> > transduce(const I& input, std::function<void(const I&)>& sink) {
		

		//Evaluate the predicate on the received input; use the boolean result
		//to decide whether or not to pass on the input.
		if (predicate(input)) {
			

			sink(input);

		}

		return std::auto_ptr<Transfer<I, I> >(0);

	}
	virtual Transfer<I, I>* clone() const {
		return new FilterTransfer<I, I_PREDICATE>(predicate);
	}
	virtual void print_tr_structure() const {
		std::cout << "filter(_)";
	}
};

/////////////////////////////////////////

template<class I_PREDICATE> Transfer<typename I_PREDICATE::argument_type, typename I_PREDICATE::argument_type>&
	_filter(I_PREDICATE predicate) {
	return *new FilterTransfer<typename I_PREDICATE::argument_type, I_PREDICATE> (predicate);
}

#define filter(PREDICATE) _filter(make_function(PREDICATE))

#endif