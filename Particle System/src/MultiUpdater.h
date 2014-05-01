/*
 * MultiUpdater.h
 *
 *  Created on: 1 maj 2014
 *      Author: Erik
 */

#ifndef MULTIUPDATER_H_
#define MULTIUPDATER_H_

#include <list>
#include <iterator>
#include "ParticleUpdater.h"

namespace Particle {

template<typename T>
class MultiUpdater : public ParticleUpdater<T> {
public:
	/**
	 * Construct a updater to which updater can be added and removed as seen fit.
	 * @param deadParticleBreaksUpdateChain Flag denoting if a "particle is dead" answer breaks the update chain
	 * and propagates the answer.
	 */
	MultiUpdater(bool deadParticleBreaksUpdateChain) {
		careForAnswer = deadParticleBreaksUpdateChain;
	}

	/**
	 * Adds a updater.
	 * @param updater The updater to add.
	 * @return
	 * False if the updater is already bound to this MultiUpdater, else false.
	 */
	bool addUpdater(ParticleUpdater<T>* updater) {
		return addToList(&updaters,updater);
	}

	/**
	 * Removes an updater.
	 * @param updater The updater to remove. This must be the exact pointer to the updater.
	 * @return
	 * True if the updater is removed. If the updater is not in the list, return false.
	 */
	bool removeUpdater(ParticleUpdater<T>* updater) {
		return removeFromList(&updaters, updater);
	}

	/**
	 * Sets how a "particle is dead" in the update chain is handled.
	 * @param flag
	 * The flag.
	 */
	void setDeadParticleBreaksUpdateChain(bool flag) {
		careForAnswer = flag;
	}

	/**
	 * Checks if a "particle is dead" message in the update chain is handled.
	 * @return
	 * True if the message breaks the update chain and propagates the answer, else false.
	 */
	bool getDeadParticleBreaksUpdateChain() {
		return careForAnswer;
	}

	bool updateParticle(T* particle) override {
		for(std::list<ParticleUpdater<T>*>::iterator iter = updaters.begin(); iter != updaters.end(); iter++) {
			if(((ParticleUpdater<T>*)*iter)->updateParticle(particle) && careForAnswer) return true;
		}
		return false;
	}

	~MultiUpdater() {
		deleteList(&initializers);
		updaters.clear();
	}

private:
	std::list<ParticleUpdater<T>*> updaters;
	bool careForAnswer;
};

}




#endif /* MULTIUPDATER_H_ */
