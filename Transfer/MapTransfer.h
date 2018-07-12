#ifndef _MAP
#define _MAP

#include <functional>
#include <memory>

template<class I, class O, class FUNCTIONAL> class MapTransfer : public Transfer<I, O> {
protected:
	FUNCTIONAL functional;
public:
	MapTransfer(const FUNCTIONAL& _functional) : functional(_functional) {
	}
	virtual ~MapTransfer() {
		;
	}
	virtual std::auto_ptr<Transfer<I, O> > transduce(const I& input,
		std::function<void(const O&)>& sink) const {
		

		sink(functional(input));


		return std::auto_ptr<Transfer<I, I> >(0);

	}
	virtual Transfer<I, O>* clone() const {
		return new MapTransfer<I, O, FUNCTIONAL>(functional);
	}
};

/////////////////////////////////////////

template<class FUNCTIONAL> Transfer<typename FUNCTIONAL::argument_type, typename FUNCTIONAL::result_type>&
	map_(FUNCTIONAL functional) {
	return *new MapTransfer<typename FUNCTIONAL::argument_type,
		typename FUNCTIONAL::result_type,
		FUNCTIONAL>(functional);
}

template<class RET, typename... ARGS> std::function<RET(ARGS...)> make_function(RET functional(ARGS...)) {
	return std::function<RET(ARGS...)>(functional);
}

/*template<class RET, typename... ARGS> std::function<RET(ARGS...)> make_function(RET(*functional)(ARGS...)) {
	return std::function<RET(ARGS...)>(functional);
}*/

template<class FUNCTION_TYPE> FUNCTION_TYPE make_function(FUNCTION_TYPE functional) {
	return functional;
}

#define map(FUNCTIONAL) (map_(make_function(FUNCTIONAL)))

#endif