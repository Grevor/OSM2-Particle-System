/*
 * Living.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef LIVING_H_
#define LIVING_H_

/**
 * Interface describing an object which is alive in some sense.
 */
class Living {
public:
	/**
	 * Checks if this object is alive. Meaning is context dependent.
	 * @return true if alive, else false.
	 */
	virtual bool isAlive() = 0;

	virtual ~Living() = 0;
};

#endif /* LIVING_H_ */
