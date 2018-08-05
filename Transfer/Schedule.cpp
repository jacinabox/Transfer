#include "stdafx.h"
#include <list>
#include <algorithm>
#include <iterator>

/*
The schedule module is how this library handles precise time, and precise time orderings.
The literature on FRP has drawn the distinction between internally and externally generated
signals. If external signal generation is fully asynchronous, time orderings of these
signals cannot be globally consistent, by the same intuition as is behind the CAP-theorem.
However, in the special case where all signals under consideration are internally generated,
all asynchronous nodes in a distributed network may replay the (statically known and calculable)
event sequences. This module handles the special case, providing globally consistent
time orderings for a special class of signals, namely internally generated ones.*/

Schedule::Schedule() : counter(0) {
}

Schedule::~Schedule() {
}

SCHEDULE_ID  Schedule::install_handler(time_t _t, std::function<void()>& _handler) {
	//Generate a schedule id.
	unsigned id = counter++;

	SCHEDULE_ENTRY se;

	se.schedule_id = id;
	se.t = _t;
	se.handler = _handler;

	//Find the proper place in the queue.
	std::list<SCHEDULE_ENTRY>::iterator it;
	for (it=schedule.begin();it!=schedule.end()&&it->t>_t;++it);

	//Insert at this place.
	schedule.insert(it, se);
	
	return id;
}

static bool remove_predicate(unsigned id, const SCHEDULE_ENTRY& se) {
	return se.schedule_id == id;
}

void Schedule::remove_handler(SCHEDULE_ID id) {
	std::function<bool(const SCHEDULE_ENTRY&)> _f(std::bind(remove_predicate, id, _1));

	std::remove_if(schedule.begin(), schedule.end(), _f);
}

unsigned Schedule::discharge_scheduled_events() {
	time_t t;

	//Decide the current time.
	time(&t);
	
	//Go through the schedule and find handlers that are due to be dispatched.
	//Dispatch and remove them.
	std::list<SCHEDULE_ENTRY>::iterator it;

	while ((it=schedule.begin())!=schedule.end()&&it->t<=t) {

		it->handler();

		schedule.pop_front();
	}

	//Indicate how long to wait for the next input event, in ms.
	return (it == schedule.end() ? -1 : static_cast<unsigned>(1000 * difftime(it->t, t)));
}