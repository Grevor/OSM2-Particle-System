/*
 * Reaper.h
 *
 *  Created on: 1 maj 2014
 *      Author: Erik
 */

#ifndef REAPER_H_
#define REAPER_H_

template<typename T>
class Reaper {
public:
	Reaper() {}

	/**
	 * Checks if a certain particle is dead.
	 * @param The particle.
	 * @return
	 * True if it is, else false.
	 */
	virtual bool isDead(T*) = 0;

	virtual ~Reaper() {}
};



#endif /* REAPER_H_ */
