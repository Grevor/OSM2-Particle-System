/*
 * MultiReaper.h
 *
 *  Created on: 1 maj 2014
 *      Author: Erik
 */

#ifndef MULTIREAPER_H_
#define MULTIREAPER_H_

#include <list>
#include <iterator>
#include "../Reaper.h"
#include "ListFunctions.h"

namespace Particle {

template<typename T>
class MultiReaper : public Reaper<T> {
public:

	/**
	 * Adds a reaper.
	 * @param reaper The reaper to add.
	 * @return
	 * False if the reaper is already bound to this MultiReaper, else false.
	 */
	bool addReaper(Reaper<T>* reaper) {
		return addToList(&reapers,reaper);
	}

	/**
	 * Removes a reaper.
	 * @param reaper The reaper to remove. This must be the exact pointer to the reaper.
	 * @return
	 * True if the reaper is removed. If the reaper is not in the list, return false.
	 */
	bool removeReaper(Reaper<T>* reaper) {
		return removeFromList(&reapers, reaper);
	}

	bool isDead(T* particle) override {
		for (typename std::list<Reaper<T>*>::iterator iter = reapers.begin(); iter < reapers.end(); ++iter) {
			if(((Reaper<T>*)*iter)->isDead(particle)) return true;
		}
		return false;
	}

	~MultiReaper() {
		deleteList(&reapers);
		reapers.clear();
	}

private:
	std::list<Reaper<T>*> reapers;
};

}



#endif /* MULTIREAPER_H_ */
