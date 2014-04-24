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
#include <boost/atomic/atomic.hpp>
#include <assert.h>

using namespace Curves;

#define PARTICLE_SYSTEM_WORK_PER_THREAD_STATIC 300

/**
 * Class representing a particle system which can be updated concurrently.
 */
template<typename Particle>
class ParticleSystem : IterationUpdateable {
public:
	ParticleSystem(ParticlePool<Particle>* pool, ParticleInitializer<Particle>* init,
			ParticleUpdater<Particle>* updater, Curve<long,long>* spawnCurve, bool looping) {
		this->pool = pool;
		this->initializer = init;
		this->updater = updater;
		this->spawnCurve = spawnCurve;
		this->loopingSystem = looping;
		this->reset();
	}

	virtual ~ParticleSystem() {

	}

	/**
	 * Resets this ParticleSystem to its starting state. Useful for reuse of ParticleSystem:s
	 */
	void reset() {
		this->currentStep = 0;
		this->alive = true;
		this->newParticlesLeftToSpawn = 0;
		if(allocationIterator != NULL) delete allocationIterator;
		if(updateIterator != NULL) delete updateIterator;
		allocationIterator = pool->getAllocationIterator();
		updateIterator = pool->getLivingIterator();
	}

	/**
	 * Updates this particle system, making it progress to its next iteration if all
	 * updating from the previous frame has been completed.
	 */
	void step() override {
		if(!hasValidIterators()) {
			updateIterator = pool->getLivingIterator();
			allocationIterator = pool->getAllocationIterator();
		}
		if(isDoneWithIteration(currentStep)) {
			this->currentStep++;
			this->newParticlesLeftToSpawn = spawnCurve->getValue(currentStep);
			updateIterator->stepComplete();
			allocationIterator->stepComplete();
		}
	}

	/**
	 * Spawns, updates and reaps particles in this ParticleSystem for this iteration.
	 * Can be called by multiple threads to perform the work concurrently.
	 */
	void update() override {
		if(updateDone() || !isAlive()) return;
		long updateStep = currentStep;
		long workunitsAllowed = PARTICLE_SYSTEM_WORK_PER_THREAD_STATIC;
		this->createNewParticles(workunitsAllowed);
		updateParticles(updateStep, workunitsAllowed);
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

	/**
	 * Checks if the current step is done.
	 * @return
	 * True if it is, else false.
	 */
	bool updateDone() {
		return !hasParticlesToSpawn() && updateIterator->hasNext();
	}

private:

	/**
	 * Checks the iterators for validity.
	 * @return
	 * True if the iterators are valid, else false.
	 */
	inline bool hasValidIterators() {
		return allocationIterator != NULL && updateIterator != NULL;
	}

	/**
	 * Attempts to acquire a particle to allocate.
	 * @return
	 * True if a particle was granted for allocation.
	 * If no more particles should be spawned, return false.
	 */
	inline bool aquireParticleToAllocate() {
		while(true) {
			long expected = newParticlesLeftToSpawn;
			if(expected <= 0) return false;
			long desired = expected - 1;
			if(newParticlesLeftToSpawn.compare_exchange_strong(&expected, desired)) return true;
		}
		return false;
	}

	/**
	 * Creates particles, up to a maximum of the specified amount.
	 * @param step The step for which to create particles.
	 * @param maxParticles The maximum number of particles to create.
	 * @return
	 * The number of particles left which this thread was allowed to
	 * spawn when spawning was complete.
	 */
	inline long createNewParticles(long maxParticles) {
		while(maxParticles > 0 && aquireParticleToAllocate()) {
			Particle* allocatedMemory = allocationIterator->next();
			if(allocatedMemory == NULL) return maxParticles; //TODO maybe throw errors, as the allocator could not find memory for us?

			initializer->initParticle(allocatedMemory);
			maxParticles--;
		}
		return maxParticles;
	}

	/**
	 * Checks if the specified iteration is done.
	 * @param step The iteration to check.
	 * @return
	 * True if the specified iteration is done, else false.
	 */
	inline bool isDoneWithIteration(long step) {
		return step != currentStep || (!hasParticlesToSpawn() && !updateIterator->hasNext());
	}

	/**
	 * Checks if this particle system has uninitialized particles which must be
	 * initialized this time-step.
	 * @return
	 * True if it has, else false.
	 */
	inline bool hasParticlesToSpawn() { return newParticlesLeftToSpawn > 0; }

	/**
	 * Updates particles in this engine.
	 * @param updateStep The update step which this thread is working on.
	 * @param maxUpdates The maximum number of updates this thread is allowed
	 * to do before control is relinquished.
	 */
	void updateParticles(long updateStep, long maxUpdates) {
		while (maxUpdates > 0 && updateIterator->hasNext() && updateStep == currentStep) {
			Particle* particleToUpdate = updateIterator->next();
			if (particleToUpdate != NULL) {
				if (updater->updateParticle(particleToUpdate)) {
					pool->returnParticle(particleToUpdate);
				}
			}
		}
	}

	ParticlePool<Particle>* pool;
	ParticleUpdater<Particle>* updater;
	ParticleInitializer<Particle>* initializer;
	ParticleIterator<Particle>* allocationIterator, *updateIterator;
	long currentStep;
	boost::atomic_int_fast64_t newParticlesLeftToSpawn;
	Curve<long,long>* spawnCurve;
	bool loopingSystem, alive;
};

#endif /* PARTICLESYSTEM_H_ */
