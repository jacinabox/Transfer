#ifndef _IDENTITY
#define _IDENTITY

#include <functional>
#include <memory>

template<class X> class IdentityTransfer : public Transfer<X, X> {
public:
	IdentityTransfer() {
	}
	virtual ~IdentityTransfer() {
	}
	virtual std::auto_ptr<Transfer<X, X> > transduce(const X& input,
		std::function<void(const X&)>& sink) const {
		sink(input);
		return std::auto_ptr<Transfer<X, X> >(new IdentityTransfer());
	}
};

//////////////////////////////////////////

#endif