#ifndef TIMER_H_
#define TIMER_H_

/**
 * A Timer keeps track of the time relative to some point in time.
 */
template<typename T>
class Timer {
public:
	/**
	 * Gets the current time according to this timer.
	 * @return
	 * The time.
	 */
	virtual T getTime() = 0;

	virtual ~Timer() {}
};

#endif /* TIMER_H_ */
