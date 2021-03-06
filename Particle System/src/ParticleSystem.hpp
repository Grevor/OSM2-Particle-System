/*
 * ParticleSystem.h
 *
 *  Created on: 16 apr 2014
 */

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "curves/Curve.h"
#include "IterationUpdateable.h"
#include "ParticleInitializer.h"
#include "ParticleIterator.h"
#include "ParticlePool.h"
#include "ReadableParticlePool.h"
#include "ParticleUpdater.h"
#include "NaiveParticlePool.hpp"
#include <stdlib.h>
#include <boost/atomic/atomic.hpp>
#include <assert.h>
#include <atomic>
#include <string.h>

using namespace Curves;
using namespace std;

#define PARTICLE_SYSTEM_WORK_PER_THREAD_STATIC 0x7fffffff

/**
 * Class representing a particle system which can be updated concurrently.
 */
template<typename Particle>
class ParticleSystem : public IterationUpdateable {
	ParticlePool<Particle>* pool;
	ParticleUpdater<Particle>* updater;
	ParticleInitializer<Particle>* initializer;
	ParticleIterator<Particle>* allocationIterator, *updateIterator;
	long currentStep;
	//boost::atomic_int_fast64_t
	atomic<int64_t> newParticlesLeftToSpawn;
	Curve<long,long>* spawnCurve;
	bool deletePool, alive;

public:
	/**
	 * Creates a new ParticleSystem with the specified components.
	 * @param pool The pool to use in this particle system.
	 * @param init The initializer to use in this particle system.
	 * @param updater The updater to use in this particle system.
	 * @param spawnCurve The spawn curve to use in this particle system.
	 * @param deletePoolWhenDone Boolean denoting whether the particle pool is to be deleted when deleting the particle system.
	 */
	ParticleSystem(ParticlePool<Particle>* pool, ParticleInitializer<Particle>* init,
			ParticleUpdater<Particle>* updater, Curve<long,long>* spawnCurve, bool deletePoolWhenDone) {
		assert(pool != NULL);
		assert(init != NULL);
		assert(updater != NULL);
		assert(spawnCurve != NULL);

		this->pool = pool;
		this->initializer = init;
		this->updater = updater;
		this->spawnCurve = spawnCurve;
		this->deletePool = deletePoolWhenDone;
		this->allocationIterator = NULL;
		this->updateIterator = NULL;
		this->alive = true;
		this->currentStep = 0;
		this->reset();
	}

	/**
	 *
	 * @param poolSize The size of the particle pool used in this particle system.
	 * @param init The initializer to use in this particle system.
	 * @param updater The updater to use in this particle system.
	 * @param spawnCurve The spawn curve to use in this particle system.
	 */
	ParticleSystem(int poolSize, ParticleInitializer<Particle>* init,
			ParticleUpdater<Particle>* updater, Curve<long,long>* spawnCurve) :
				ParticleSystem(new NaiveParticlePool<Particle>(poolSize), init, updater, spawnCurve, true) {}

	virtual ~ParticleSystem() {
		if(deletePool) delete pool;
	}

	/**
	 * Gets the maximum number of particles that this particle system can hold.
	 * @return Maximum number of particles this system can hold.
	 */
	int getMaxSize() {
		return pool->size();
	}

	/**
	 * Resets this ParticleSystem to its starting state. Useful for reuse of ParticleSystems.
	 * May NOT be called while the particle system is being updated.
	 */
	void reset() {
		this->currentStep = 0;
		this->alive = true;
		this->newParticlesLeftToSpawn = 0;
		if(allocationIterator != NULL) delete allocationIterator;
		if(updateIterator != NULL) delete updateIterator;
		pool->reset();
		allocationIterator = pool->getAllocationIterator();
		updateIterator = pool->getUpdateIterator();
	}

	/**
	 * Updates this particle system, making it progress to its next iteration after all
	 * updating from the previous frame has been completed.
	 *
	 * Note that this is a blocking call.
	 */
	void step() override {
		while(true) {
			if(isDoneWithIteration(currentStep)) {
				this->currentStep++;
				this->newParticlesLeftToSpawn = spawnCurve->getValue(currentStep);
				updateIterator->stepComplete();
				allocationIterator->stepComplete();
				pool->stepComplete();
				return;
			}
		}
	}

	/**
	 * Checks if the current step is complete or not.
	 * Please note that this function may miss the updating and/or creation of up to n - 1 particles,
	 * where n is the number of threads updating the system.
	 * @return True if the step is complete, else false.
	 */
	bool isStepComplete() override {
		return isDoneWithIteration(currentStep);
	}

	/**
	 * Spawns, updates and reaps particles in this ParticleSystem for this iteration.
	 * Can be called by multiple threads to perform the work concurrently.
	 */
	void update() override {
		if(updateDone() || !isAlive()) return;
		long updateStep = currentStep;
		long workunitsAllowed = PARTICLE_SYSTEM_WORK_PER_THREAD_STATIC;
		workunitsAllowed = this->createNewParticles(workunitsAllowed);
		updateParticles(updateStep, workunitsAllowed);
	}

	/**
	 * Checks if this particle system is alive.
	 * @return true if alive, else false.
	 */
	virtual bool isAlive() override {
		return alive;
	}

	/**
	 * Sets the alive flag of this particle system.
	 * A particle system which is not alive will stop being updated, but may reside in memory until later.
	 * @param flag The value to set.
	 */
	void setAlive(bool flag) {
		alive = flag;
	}

	/**
	 * Checks if the current step is done.
	 * @return
	 * True if it is, else false.
	 */
	bool updateDone() {
		return !hasParticlesToSpawn() && !updateIterator->hasNext();
	}

	/**
	 * Gets the living particles of a particle-system.
	 * @return
	 */
	ParticleIterator<Particle>* getLivingParticles() {
		return pool->getLivingIterator();
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
			int64_t expected = newParticlesLeftToSpawn;
			if(expected <= 0)
				return false;
			int64_t desired = expected - 1;
			if(std::atomic_compare_exchange_strong(&this->newParticlesLeftToSpawn, &expected, desired)) return true;
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
			if(allocatedMemory == NULL) {
				this->newParticlesLeftToSpawn = 0;
				return maxParticles; //TODO maybe throw errors, as the allocator could not find memory for us?
			}

			initializer->initParticle(allocatedMemory);
			maxParticles--;
			allocationIterator->done(allocatedMemory);
		}
		return maxParticles;
	}

	/**
	 * Checks if the specified iteration is done.
	 * This function may currently miss up to n - 1 particles,
	 * where n is the number of threads updating the system.
	 * @param step The iteration to check.
	 * @return
	 * True if the specified iteration is done, else false.
	 */
	inline bool isDoneWithIteration(long step) {
		return !hasParticlesToSpawn() && !updateIterator->hasNext();
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
				} else {
					updateIterator->done(particleToUpdate);
				}
			}
		}
	}


};

#endif /* PARTICLESYSTEM_H_ */
