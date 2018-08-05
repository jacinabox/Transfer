#ifndef _SCHEDULE
#define _SCHEDULE

#include <list>
#include <functional>
#include <memory>
#include <utility>
#include <time.h>
#include <assert.h>

typedef unsigned SCHEDULE_ID;

struct SCHEDULE_ENTRY {
	unsigned schedule_id;
	time_t t;
	std::function<void()> handler;
};

class Schedule {
protected:
	unsigned counter;
	std::list<SCHEDULE_ENTRY> schedule;
public:
	Schedule();
	virtual ~Schedule();
	virtual SCHEDULE_ID  install_handler(time_t _t, std::function<void()>& _handler);
	virtual void remove_handler(SCHEDULE_ID id);
	virtual unsigned discharge_scheduled_events();
};

//////////////////////////////////



template<class T> class SignalObservable : public Observable<std::pair<T, time_t> >{
protected:
	std::auto_ptr<Transfer<Nothing, std::pair<T, time_t> > > transfer;
	std::pair<T, time_t> next_output;
	Schedule& schedule;
	std::function<void(const std::pair<T, time_t>&)> handler;

	void signal_observable_helper(const std::pair<T, time_t>& output) {
		std::function<void()> _f(std::bind(&SignalObservable<T>::signal_observable_helper2, this));
		next_output = output;

	
		schedule.install_handler(output.second, _f);
	}

	void signal_observable_helper2() {
		std::function<void(const std::pair<T, time_t>&)> _f(std::bind(&SignalObservable<T>::signal_observable_helper, this, _1));
		
		handler(next_output);

		
		ptr_assignment_helper(transfer, transfer, transfer->transduce(Nothing(), _f));
	}

public:
	SignalObservable(std::auto_ptr<Transfer<Nothing, std::pair<T, time_t> > > _transfer, Schedule& _schedule) : transfer(_transfer), schedule(_schedule) {
		assert(transfer.get());
	}

	virtual ~SignalObservable() {
	}

	virtual void install_handler(std::function<void(const std::pair<T, time_t>&)> _handler) {
		handler = _handler;

		//Once handler is installed begin the callback recursion.
		std::function<void(const std::pair<T, time_t>&)> _f(std::bind(&SignalObservable<T>::signal_observable_helper, this, _1));
		ptr_assignment_helper(transfer, transfer, transfer->transduce(Nothing(), _f));
	}

};

//Wrapper for SignalObservable constructor.
template<class T> Observable<std::pair<T, time_t> >& signal(Transfer<Nothing, std::pair<T, time_t> >& transfer, Schedule& schedule) {
	return *new SignalObservable(std::auto_ptr < Transfer<Nothing, std::pair<T, time_t> >(&transfer), schedule);
}

#endif
