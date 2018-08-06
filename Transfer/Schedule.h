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


/*
The schedule module is how this library handles precise time, and precise time orderings.
The literature on FRP has drawn the distinction between internally and externally generated
signals. If external signal generation is fully asynchronous, time orderings of these
signals cannot be globally consistent, by the same intuition as is behind the CAP-theorem.
However, in the special case where all signals under consideration are internally generated,
all asynchronous nodes in a distributed network may replay the ( known and calculable)
event sequences. This module handles the special case, providing globally consistent
time orderings for a special class of signals, that is internally generated ones.*/
class Schedule {
protected:
	unsigned counter;
	std::list<SCHEDULE_ENTRY> schedule;
public:
	Schedule();
	virtual ~Schedule();
	//Methods to install or remove handlers. Each handler is trackable by its schedule id.
	virtual SCHEDULE_ID  install_handler(time_t _t, std::function<void()>& _handler);
	virtual void remove_handler(SCHEDULE_ID id);
	//When called at regular intervals, accomplishes dispatching onto handlers.
	virtual unsigned dispatch_scheduled_events();
};

//////////////////////////////////
