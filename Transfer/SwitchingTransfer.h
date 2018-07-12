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

#define SWITCH_INPUT_TYPES(I, O) <std::auto_ptr<Transfer<I, O> >, I>
#define SWITCH_INPUT(I, O) Sum SWITCH_INPUT_TYPES(I, O)

template<class I, class O> class SwitchingTransfer : public Transfer<SWITCH_INPUT(I, O), O> {
protected:
	std::auto_ptr<Transfer<I, O> > transfer;

public:
	SwitchingTransfer(std::auto_ptr<Transfer<I, O> > _transfer) : transfer(_transfer) {
		assert(transfer.get());
	}

	virtual ~SwitchingTransfer() {
	}

	//Based on Yampa's switching combinator of arrow type ((I ~> O)+I) ~> O.
	virtual std::auto_ptr<Transfer<SWITCH_INPUT(I, O), O> > transduce(const SWITCH_INPUT(I, O)& input,
		std::function<void(const O&)>& sink) const {
		std::auto_ptr<Transfer<I, O> >* left_data;
		I* right_data;
		std::auto_ptr<Transfer<I, O> > transfer2;
		SWITCH_INPUT(I, O)& p = const_cast<SWITCH_INPUT(I, O)&>(input);

		right_data = p.retrieve_right_data();
		if (right_data) {
			transfer2 =  transfer->transduce(*right_data, sink);
			if (!transfer2.get()) transfer2.reset(transfer->clone());
		}
		else {
			left_data = p.retrieve_left_data();
			assert(left_data);
			transfer2 = *left_data;
		}

		return std::auto_ptr<Transfer<SWITCH_INPUT(I, O), O> >(new SwitchingTransfer<I, O>(transfer2));
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<SWITCH_INPUT(I, O), O>* clone() const {
		return new SwitchingTransfer<I, O>(std::auto_ptr<Transfer<I, O> >(transfer->clone()));
	}
};

///////////////////////////////////////

template<class I, class O> Transfer<SWITCH_INPUT(I, O), O>& r_switch(Transfer<I, O>& transfer) {
	return *new SwitchingTransfer<I, O> (std::auto_ptr<Transfer<I, O> >(&transfer));
}

#endif