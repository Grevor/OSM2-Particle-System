/*
 * MultiInitializer.h
 *
 *  Created on: 1 maj 2014
 *      Author: Erik
 */

#ifndef MULTIINITIALIZER_H_
#define MULTIINITIALIZER_H_

#include <list>
#include <iterator>

#include "../ParticleInitializer.h"
#include "ListFunctions.h"


namespace Particle {

template<typename T>
class MultiInitializer : public ParticleInitializer<T> {
public:
	MultiInitializer() { }

	/**
	 * Removes an initializer.
	 * @param init The initializer to remove. This must be the exact pointer to the initializer.
	 * @return
	 * True if the initializer is removed. If the initializer is not in the list, return false.
	 */
	bool removeInitializer(ParticleInitializer<T>* init) {
		return removeFromList(&initializers,init);
	}

	/**
	 * Adds an initializer.
	 * @param init The initializer to add.
	 * @return
	 * False if the initializer is already bound to this MultiInitializer, else false.
	 */
	bool addInitializer(ParticleInitializer<T>* init) {
		return addToList(&initializers,init);
	}

	void initParticle(T* particle) override {
		for(typename std::list<ParticleInitializer<T>*>::iterator iter = initializers.begin(); iter != initializers.end(); iter++) {
			((ParticleInitializer<T>*)*iter)->initParticle(particle);
		}
	}

	~MultiInitializer() {
		deleteList(&initializers);
		initializers.clear();
	}

private:
	std::list<ParticleInitializer<T>*> initializers;
};

}



#endif /* MULTIINITIALIZER_H_ */
