/*
 * StandardTimer.hpp
 *
 *  Created on: 6 maj 2014
 */

#ifndef STANDARDTIMER_HPP_
#define STANDARDTIMER_HPP_

#include "../utilities/Timer.h"
#include <boost/chrono.hpp>
#include <time.h>

using namespace boost::chrono;

/**
 * Timer returning the time since some reference time (or timer creation) in seconds.
 * This timer is only exact down to milliseconds.
 */
class StandardTimer : Timer<float> {
	system_clock::time_point startTime;
public:

	StandardTimer() {
		setCurrentTimeAsReferenceTime();
	}

	float getTime() override {
		duration<float> sec = system_clock::now() - startTime;
		return sec.count();
	}

	void setReferenceTime(system_clock::time_point newReference) {
		this->startTime = newReference;
	}

	void setCurrentTimeAsReferenceTime() {
		setReferenceTime(system_clock::now());
	}
};



#endif /* STANDARDTIMER_HPP_ */
