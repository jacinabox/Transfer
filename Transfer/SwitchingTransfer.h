#ifndef _SWITCHING
#define _SWITCHING

#include <functional>
#include <memory>
#include <assert.h>

template<class T, class U> struct Sum {
	Sum() {
		throw "Cannot construct the base class";
	}
};

template<class T, class U> struct Left : public Sum<T, U> {
	T data;

	Left(T& _data) : data(_data) {
	}
};

template<class T, class U> struct Right : public Sum<T, U> {
	U data;

	Right(U& _data) : data(_data) {
	}
};

//The official solution to decomposing sum types involves a lot of rigmarole
//with the visitor pattern; this is a low tech solution; the retrieval
//functions return a pointer to data; a null pointer signifies the data
//is not there.
template<class T, class U> T* retrieve_left_data(Sum<T, U>& sum) {
	Left<T, U>* sum2 = dynamic_cast<Left<T, U>*>(*sum);

	if (sum2) return &(sum2->data);
	return 0;
}

template<class T, class U> U* retrieve_right_data(Sum<T, U>& sum) {
	Right<T, U>* sum2 = dynamic_cast<Right<T, U>*>(*sum);

	if (sum2) return &(sum2->data);
	return 0;
}

/////////////////////////////////

#define SWITCH_INPUT(I, O) Sum<std::auto_ptr<Transfer<I, O> >, I>

template<class I, class O> class SwitchingTransfer : public Transfer<SWITCH_INPUT(I, O), O> {
protected:
	std::auto_ptr<Transfer<I, O> > transfer;

public:
	SwitchingTransfer(std::auto_ptr<Transfer<I, O> >& _transfer) : transfer(_transfer) {
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

		right_data = retrieve_right_data(input);
		if (right_data) {
			transfer2 = transfer->transduce(*right_data, sink);
		}
		else {
			left_data = retrieve_left_data(input);
			assert(left_data);
			transfer2 = *left_data;
		}
		return std::auto_ptr<Transfer<SWITCH_INPUT(I, O), O> >(new SwitchingTransfer(transfer2));
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<SWITCH_INPUT(I, O), O>* clone() const {
		return new SwitchingTransfer(transfer.clone());
	}
};

#endif