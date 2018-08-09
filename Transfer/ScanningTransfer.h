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


	ScanningTransfer(const T& _itl, const T_I_FUNCTION& _f) : itl(_itl), f(_f) {
	}
	virtual ~ScanningTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, T> > transduce(const I& input, std::function<void(const T&)>& sink) {
		T itl2 = f(itl, input);
		sink(itl2);
		return std::auto_ptr<Transfer<I, T> > (new ScanningTransfer(itl2, f));
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<I, T>* clone() const {
		return new ScanningTransfer<I, T, T_I_FUNCTION>(itl, f);
	}
	virtual void print_tr_structure() const {
		std::cout << "scanning(_,_)";
	}
};


///////////////////////////////////////////

template<class T, class T_I_FUNCTION> Transfer<typename T_I_FUNCTION::second_argument_type, T>&
	_scanning(const T& _itl, T_I_FUNCTION _f) {
	return *new ScanningTransfer<typename T_I_FUNCTION::second_argument_type, T, T_I_FUNCTION>(_itl, _f);
}

#define scanning(ITL, F) (_scanning(ITL, make_function(F)))

#endif
