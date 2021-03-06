#ifndef _WITH_REMAINDER
#define _WITH_REMAINDER

#include <utility>
#include <memory>
#include <functional>

#define WITH_REMAINDER_OUTPUT(I, O) std::pair<std::auto_ptr<Transfer<I, O> >,O>

template<class I, class O> void with_remainder_helper(const std::auto_ptr<Transfer<I, O> >* p_transfer,
	std::function<void(const WITH_REMAINDER_OUTPUT(I, O)&)>& sink,
	const I& input) {
		sink(std::make_pair(std::auto_ptr<Transfer<I, O> > ((*p_transfer)->clone()), input));
}

/*A transfer wrapper(revise) to run its inner transfer and also supply a cloned copy of this transfer
with each output emitted. Such copy can be used to resume elsewhere in the transfer network,
such as in a switch.*/
template<class I, class O> class WithRemainderTransfer : public Transfer<I, WITH_REMAINDER_OUTPUT(I, O)> {
protected:
	std::auto_ptr<Transfer<I, O> > transfer;
public:
	WithRemainderTransfer(std::auto_ptr<Transfer<I, O> > _transfer) : transfer(_transfer) {
		assert(transfer.get());
	}
	virtual ~WithRemainderTransfer() {
	}
	virtual std::auto_ptr<Transfer<I, WITH_REMAINDER_OUTPUT(I, O)> > transduce(
		const I& input,
		std::function<void(const WITH_REMAINDER_OUTPUT(I, O)&)>& sink)
		{
		std::function<void(const O&)> f(std::bind(with_remainder_helper<I, O>, &transfer, sink, _1));


		ptr_assignment_helper(transfer, transfer, transfer->transduce(input, f));



		//if (!new_transfer.get()) new_transfer.reset(transfer->clone());


		return std::auto_ptr<Transfer<I, WITH_REMAINDER_OUTPUT(I, O)> >
			(new WithRemainderTransfer<I, O>(transfer));
	}
	virtual bool is_stateless() const {
		return false;
	}
	virtual Transfer<I, WITH_REMAINDER_OUTPUT(I, O)>* clone() const {
		return new WithRemainderTransfer<I, O>(std::auto_ptr<Transfer<I, O> >(transfer->clone()));
	}
	virtual void print_tr_structure() const {
		std::cout << "with_remainder(";
		transfer->print_tr_structure();
		std::cout << ")";
	}
};

///////////////////////////////////////

template<class I, class O> Transfer<I, WITH_REMAINDER_OUTPUT(I, O)>& with_remainder(Transfer<I, O>& _transfer) {
	return *new WithRemainderTransfer<I, O>(std::auto_ptr<Transfer<I, O> >(&_transfer));
}

#endif
