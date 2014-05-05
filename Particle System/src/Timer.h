/*
 * Timer.h
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef TIMER_H_
#define TIMER_H_

template<typename T>
class Timer {
public:
	/**
	 * Gets the current time accordin to this timer.
	 * @return
	 * The time.
	 */
	virtual T getTime() = 0;

	virtual ~Timer() {}
};



#endif /* TIMER_H_ */
