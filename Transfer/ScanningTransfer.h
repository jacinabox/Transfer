#ifndef _SCANNING
#define _SCANNING

#include <functional>
#include <memory>

template<class I, class  T, class T_I_FUNCTION> class ScanningTransfer :
	public Transfer<I, T> {
protected:
	T itl;
	T_I_FUNCTION f;

public:
	ScanningTransfer(T& _itl, const T_I_FUNCTION& _f) : itl(_itl), f(_f) {
	}
	virtual ~ScanningTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, T> > transduce(const I& input, std::function<void(const T&)>& sink)const {
		T itl2 = f(itl, input);
		sink(itl2);
		return std::auto_ptr<Transfer<I, T> > (new ScanningTransfer(itl2, f));
	}
};


///////////////////////////////////////////

template<class I, class T, class T_I_FUNCTION> Transfer<I, T>* scanning(const I& _type_determiner, T& _itl, T_I_FUNCTION& _f) {
	return new ScanningTransfer<I, T, T_I_FUNCTION>(_itl, _f);
}

#endif