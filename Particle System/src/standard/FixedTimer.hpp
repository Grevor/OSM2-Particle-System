/*
 * FixedTimer.hpp
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef FIXEDTIMER_HPP_
#define FIXEDTIMER_HPP_
#include "../Timer.h"

template<typename T>
class FixedTimer : public Timer<T> {
	T time;

public:
	FixedTimer(T time) {
		this->time = time;
	}

	T getTime() override {
		return time;
	}

	void setTime(T t) {
		time = t;
	}
};



#endif /* FIXEDTIMER_HPP_ */
