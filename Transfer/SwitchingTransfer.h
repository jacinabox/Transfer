#ifndef _SWITCHING
#define _SWITCHING

#include <functional>
#include <memory>
#include <assert.h>

template<class T, class U> class Sum {
private:
	std::auto_ptr<T> data_left;
	std::auto_ptr<U> data_right;

public:
	~Sum() {
	}

	T* retrieve_left_data() {
		return data_left.get();
	}

	const T* retrieve_left_data() const {
		return data_left.get();
	}

	U* retrieve_right_data() {
		return data_right.get();
	}

	const T* retrieve_right_data() const {
		return data_right.get();
	}

	//This is a low tech solution; the retrieval
	//functions return a pointer to data; a null pointer signifies the data
	//is not there.
	static Sum<T, U> left_(const T& x) {
		Sum<T, U> sum;

		sum.data_left = std::auto_ptr<T>(new T(x));
		return sum;
	}

	static Sum<T, U> right_(const U& x) {
		Sum<T, U> sum;

		sum.data_right = std::auto_ptr<U>(new U(x));
		return sum;
	}
};

template<class T, class U> bool is_left(Sum<T, U>& sum) {
	return !!sum.retrieve_left_data();
}

template<class T, class U> bool is_right(Sum<T, U>& sum) {
	return !!sum.retrieve_right_data();
}

template<class T, class U> T unsafe_retrieve_left_data(Sum<T, U> sum) {
	return *const_cast<Sum<T, U>& > (sum).retrieve_left_data();
}

template<class T, class U> U unsafe_retrieve_right_data(Sum<T, U> sum) {
	return *const_cast<Sum<T, U>&> (sum).retrieve_right_data();
}


/////////////////////////////////




template<class I, class O> class SwitchingTransfer : public Transfer<I, O> {
protected:
	mutable std::auto_ptr<Transfer<I, O> > transfer;
	std::function<std::auto_ptr<Transfer<I, O> >(I)> transfer_f;

public:
	//The functional argument returns the transfer to switch to, or null pointer if no switch is desired.
	//Upon switching this transfer alters its type to that of the transfer being switched to; i.e.
	//it returns the transfer being switched to, from transduce.
	SwitchingTransfer(std::auto_ptr<Transfer<I, O> > _transfer,
		std::function<std::auto_ptr<Transfer<I, O> >(I)> _transfer_f) : transfer(_transfer), transfer_f(_transfer_f) {
	}

	virtual ~SwitchingTransfer() {
	}

	//Based on Yampa's one shot-switching combinator.
	virtual std::auto_ptr<Transfer<I, O> > transduce(I input,
		std::function<void(const O&)>& sink) const {
		std::auto_ptr<Transfer<I, O> > transfer2 = transfer_f(input);
		std::auto_ptr<Transfer<I, O> > transfer3;

		ptr_assignment_helper(transfer3, transfer, transfer->transduce(input, sink));
		if (transfer2.get()) {
			return transfer2;
		}
		else {
			return std::auto_ptr<Transfer<I, O> >(new SwitchingTransfer<I, O>(transfer3, transfer_f));
		}
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<I, O>* clone() const {
		return new SwitchingTransfer<I, O>(std::auto_ptr<Transfer<I, O> >(transfer->clone()), transfer_f);
	}
};

///////////////////////////////////////

template<class I, class O> Transfer<I, O>& switch_once(Transfer<I, O>& transfer,
	std::function<std::auto_ptr<Transfer<I, O> >(I)> transfer_f) {
	return *new SwitchingTransfer<I, O> (std::auto_ptr<Transfer<I, O> >(&transfer), transfer_f);
}

//////////////////////////////////////
// Bootstrapping to a repeated switch by recursive definition.

template<class I, class O> std::auto_ptr<Transfer<I, O> > r_switch_helper(std::function<std::auto_ptr<Transfer<I, O> >(I)> transfer_f,
	I input) {
	Transfer<I, O>* p_temp = transfer_f(input).release();

	return std::auto_ptr<Transfer<I, O> >(p_temp ? &r_switch(transfer_f, *p_temp) : 0);
}

template<class I, class O> Transfer<I, O>& r_switch(std::function<std::auto_ptr<Transfer<I, O> >(I)> transfer_f,
	Transfer<I, O>& transfer) {
	std::function<std::auto_ptr<Transfer<I, O> >(I)> _f(std::bind(r_switch_helper<I, O>, transfer_f, _1));

	return switch_once<I, O>(transfer, _f);
}

#endif