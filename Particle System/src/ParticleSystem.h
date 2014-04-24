/*
 * ParticleSystem.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "Curve.h"
#include <stdlib.h>

using namespace Curves;

/**
 * Class representing a particle system which can be updated concurrently.
 */
template<typename Particle>
class ParticleSystem : IterationUpdateable {
public:
	ParticleSystem();
	virtual ~ParticleSystem();
	/**
	 * Resets this ParticleSystem to its starting state. Useful for reuse of ParticleSystem:s
	 */
	void reset() {

	}
	/**
	 * Updates this particle system, making it progress to its next iteration if all
	 * updating from the previous frame has been completed.
	 */
	void step() {
		if(!hasValidIterators()) {
			updateIterator = pool->getLivingIterator();
			allocationIterator = pool->getAllocationIterator();
		}
		if(hasValidIterators() && !updateIterator->hasNext()) {
			this->currentStep++;

			updateIterator->stepComplete();
			allocationIterator->stepComplete();

		}
	}
	/**
	 * Spawns, updates and reaps particles in this ParticleSystem for this iteration.
	 * Can be called by multiple threads to perform the work concurrently.
	 */
	void updateParticles() {
		if(!hasValidIterators() || !isAlive()) return;
		long updateStep = currentStep;
		while(updateIterator->hasNext() && updateStep == currentStep) {
			Particle* particleToUpdate = updateIterator->next();
			if(particleToUpdate != NULL) {
				if(updater->updateParticle(particleToUpdate))
					pool->returnParticle(particleToUpdate);
			}
		}
	}
	/**
	 * Check if this ParticleSystem is looping, indicating that it will keep spawning
	 * particles and never die, unless it's forced to do so.
	 * @return true if looping, else false
	 */
	bool isLooping() {
		return loopingSystem;
	}

	virtual bool isAlive() override {
		return alive;
	}


private:
	inline bool hasValidIterators() {
		return allocationIterator != NULL && updateIterator != NULL;
	}

	ParticlePool<Particle>* pool;
	ParticleUpdater<Particle>* updater;
	ParticleInitializer<Particle>* initializer;
	ParticleIterator<Particle>* allocationIterator, *updateIterator;
	long currentStep;
	Curve<long,long>* spawnCurve;
	bool loopingSystem, alive;
};

#endif /* PARTICLESYSTEM_H_ */
