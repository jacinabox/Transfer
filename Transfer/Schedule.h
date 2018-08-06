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
