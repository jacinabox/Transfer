#ifndef _SCANNING
#define _SCANNING

#include <functional>
#include <memory>

template<class I, class  T, class T_I_FUNCTION> class ScanningTransfer :
	public Transfer<I, T> {
protected:
	T itl;
	T_I_FUNCTION f;
	bool stateless;

public:
	//Boolean variable indicates whether or not this scan is being used as a map,
	//in which case it is stateless.
	ScanningTransfer(const T& _itl, const T_I_FUNCTION& _f, bool _stateless=false) : itl(_itl), f(_f), stateless(_stateless) {
	}
	virtual ~ScanningTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, T> > transduce(const I& input, std::function<void(const T&)>& sink)const {
		T itl2 = f(itl, input);
		sink(itl2);
		return std::auto_ptr<Transfer<I, T> > (new ScanningTransfer(itl2, f));
	}
	virtual bool is_stateless() const {
		return stateless;
	}
	virtual Transfer<I, T>* clone() const {
		return new ScanningTransfer<I, T, T_I_FUNCTION>(itl, f, stateless);
	}
};


///////////////////////////////////////////

template<class I, class T, class T_I_FUNCTION> Transfer<I, T>& scanning(const I& _type_determiner, const T& _itl, T_I_FUNCTION& _f, bool _stateless = false) {
	return *new ScanningTransfer<I, T, T_I_FUNCTION>(_itl, _f, _stateless);
}

#endif