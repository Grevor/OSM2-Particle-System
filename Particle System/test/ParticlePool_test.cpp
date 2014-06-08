/*
 * ParticlePool_test.h
 *
 *  Created on: 29 apr 2014
 *  [THIS IS NOT AN AUTOMATIC TEST]
 *  The following file contains no cool framework or otherwise. It simply contains methods for testing ParticlePools.
 *  The general promise of these functions is that if they do not generate a SIGINT through assert(), they pass.
 *  Please note the comments of the corresponding test-functions when using them.
 *
 *  These tests work only under cygwin.
 */

#ifndef PARTICLEPOOL_TEST_H_
#define PARTICLEPOOL_TEST_H_

#include "DebugHelp.h"
#include "../src/ParticlePool.h"
#include "../src/ParticleIterator.h"
#include <stdlib.h>

namespace Test {

/**
 * Tests that particles flow the was they are supposed to in the pool when dealing with its iterators.
 * @param pool The pool to check. This pool must have a size of AT LEAST 3 particles, or the test will fail.
 */
template<typename T> void testParticlePoolInitialize(ParticlePool<T>* pool) {

	TEST_ASSERT_TRUE(pool != NULL);
	ParticleIterator<T>* allocationIterator = pool->getAllocationIterator();
	ParticleIterator<T>* livingIterator = pool->getLivingIterator();
	ParticleIterator<T>* updateIterator = pool->getUpdateIterator();

	T* particle;
	T* oldParticle;

	//Allocates a particle and makes sure it is made available to the livingIterator AND the updateIterator.
	TEST_ASSERT_TRUE(allocationIterator->hasNext());
	particle = allocationIterator->next();
	TEST_ASSERT_TRUE(particle != NULL);
	TEST_ASSERT_FALSE(livingIterator->hasNext());
	TEST_ASSERT_FALSE(updateIterator->hasNext());
	allocationIterator->done(particle);
	TEST_ASSERT_TRUE(livingIterator->hasNext());
	TEST_ASSERT_TRUE(updateIterator->hasNext());

	//retrieves the same particle from the update-iterator, asserts that it is non-null (per iterator standards),
	//that it is the same particle that was just created, and asserts that the update iterator no longer has a next particle.
	oldParticle = particle;
	particle = updateIterator->next();
	TEST_ASSERT_TRUE(particle != NULL);
	TEST_ASSERT_EQUAL(particle, oldParticle);
	TEST_ASSERT_FALSE(updateIterator->hasNext());
	TEST_ASSERT_FALSE(livingIterator->hasNext());
	updateIterator->done(particle);
	TEST_ASSERT_TRUE(livingIterator->hasNext());
	TEST_ASSERT_TRUE(updateIterator->hasNext());
	bool hadAdditionalAllocation = allocationIterator->hasNext();

	// Retrieves the same particle yet again from the living iterator, asserts that it is non-null (per iterator standards),
	//that it is the same particle that was just updated, and asserts that the living iterator no longer has a next particle.
	//Also checks that no allocator state is changed when retrieving/returning particles but the living iterator.
	particle = livingIterator->next();
	TEST_ASSERT_TRUE(particle != NULL);
	TEST_ASSERT_EQUAL(particle, oldParticle);
	TEST_ASSERT_FALSE(livingIterator->hasNext());
	TEST_ASSERT_FALSE(updateIterator->hasNext());
	TEST_ASSERT_EQUAL(allocationIterator->hasNext(), hadAdditionalAllocation);
	livingIterator->done(particle);
	TEST_ASSERT_FALSE(livingIterator->hasNext());
	TEST_ASSERT_FALSE(updateIterator->hasNext());
	TEST_ASSERT_EQUAL(allocationIterator->hasNext(), hadAdditionalAllocation);

	oldParticle = allocationIterator->next();
	TEST_ASSERT_TRUE(oldParticle != NULL);
	allocationIterator->done(oldParticle);

	T* oldParticle2 = allocationIterator->next();
	TEST_ASSERT_TRUE(oldParticle != NULL);
	allocationIterator->done(oldParticle2);

	TEST_ASSERT_TRUE(oldParticle == livingIterator->next());
	livingIterator->done(oldParticle);

	TEST_ASSERT_TRUE(oldParticle == livingIterator->next());
	livingIterator->done(oldParticle2);

	/**
	 * Tests that iterators detect NULL-pointers and ignore them.
	 */
	allocationIterator->done(NULL);
	TEST_ASSERT_FALSE(updateIterator->hasNext());

	updateIterator->done(NULL);
	TEST_ASSERT_FALSE(livingIterator->hasNext());

	delete allocationIterator;
	delete livingIterator;
	delete updateIterator;
}

/**
 * Tests that the returnParticle function works as intended. This function assumes that the pool
 * can run out of particles and that allocation can fail.
 *
 * [NOTICE] This test assumes that the pool contains at least 100 particles.
 * @param pool The pool to test.
 * @param poolReturnsParticlesOnlyAtEndOfStep Denotes if the pool must be signaled with a step complete before returned
 * particles are made visible to allocators.
 */
template<typename T> void testParticlePoolReturn(ParticlePool<T>* pool, bool poolReturnsParticlesOnlyAtEndOfStep) {
	const int returnTestSize = 100;
	T* particles[returnTestSize];
	int currentParticle = 0;
	ParticleIterator<T>* allocator = pool->getAllocationIterator();
	while(allocator->hasNext()) {
		particles[currentParticle] = allocator->next();
		allocator->done(particles[currentParticle]);
		currentParticle = ++currentParticle % returnTestSize;
	}

	/**
	 * Tests that all 100 particles display the same behavior,
	 * and that the edge-case of the pool being empty and only one element is returned works.
	 */
	for(int i = 0; i < returnTestSize; i++) {
		TEST_ASSERT_FALSE(allocator->hasNext());
		TEST_ASSERT_TRUE(particles[i] != NULL);
		pool->returnParticle(particles[i]);
		if(poolReturnsParticlesOnlyAtEndOfStep) pool->stepComplete();
		TEST_ASSERT_TRUE(allocator->hasNext());
		TEST_ASSERT_EQUAL(allocator->next(), particles[i]);
		allocator->done(particles[i]);
		TEST_ASSERT_FALSE(allocator->hasNext());
	}

	/**
	 * Tests that a returned NULL-pointer is detected as a faulty return.
	 */
	TEST_ASSERT_FALSE(allocator->hasNext());
	pool->returnParticle(NULL);
	TEST_ASSERT_FALSE(allocator->hasNext());

	/**
	 * Returns all 100 particles in one go and checks that none are leaked or faulty pointed by the pool.
	 * Also makes sure that we indeed have no particles in the pool at the end of the returning and checking.
	 */
	for(int i = 0; i < returnTestSize; i++) {
		pool->returnParticle(particles[i]);
		if(poolReturnsParticlesOnlyAtEndOfStep) pool->stepComplete();
		TEST_ASSERT_TRUE(allocator->hasNext());
	}

	for(int i = 0; i < returnTestSize; i++) {
		TEST_ASSERT_TRUE(allocator->hasNext());
		T* particle = allocator->next();
		TEST_ASSERT_TRUE(isInArray(particles,returnTestSize,particle));
		allocator->done(particle);
	}
	TEST_ASSERT_FALSE(allocator->hasNext());

	delete allocator;
}

template<typename T> void testIteratorsDone(ParticlePool<T>* pool) {

}

/**
 * Tests that the reset function does not leak in any way.
 * @param pool The pool to test.
 * @param canBecomeEmpty Denotes if the pool can become full. If this is false, the pool is thought to be looping in some maner.
 */
template<typename T> void testReset(ParticlePool<T>* pool, bool canBecomeEmpty) {
	const int resetTestIterations = 20;

	/**
	 * Tests that the same amount of particles are attained after each reset. That is: no particles are leaked at reset.
	 */
	int oldResult = -1;
	for(int i = 0; i < resetTestIterations; i++) {
		if(oldResult == -1) {
			oldResult = getParticlesBeforeLoopOrEmpty(pool, canBecomeEmpty);
		}
		else {
			pool->reset();
			TEST_ASSERT_EQUAL(oldResult, getParticlesBeforeLoopOrEmpty(pool, canBecomeEmpty));
		}
	}

}

/**
 * Gets the number of particles that a pool can curn out before either looping or becoming empty.
 * @param pool The pool to test.
 * @param canBecomeEmpty Denotes if the pool can become empty. If not, the test assumes it is a looping pool of some kind.
 * @return
 */
template<typename T> int getParticlesBeforeLoopOrEmpty(ParticlePool<T>* pool, bool canBecomeEmpty) {
	ParticleIterator<T>* allocator = pool->getAllocationIterator();
	ParticleIterator<T>* living = pool->getLivingIterator();
	ParticleIterator<T>* updater = pool->getUpdateIterator();
	TEST_ASSERT_TRUE(allocator != NULL);
	TEST_ASSERT_TRUE(allocator->hasNext());
	TEST_ASSERT_TRUE(living != NULL);
	TEST_ASSERT_FALSE(living->hasNext());
	TEST_ASSERT_TRUE(updater != NULL);
	TEST_ASSERT_FALSE(updater->hasNext());
	T* lastExpectedParticle = NULL; //This is the standard value to return if no allocation can be made.

	if(canBecomeEmpty) {
		lastExpectedParticle = allocator->next();
		TEST_ASSERT_TRUE(lastExpectedParticle != NULL);
		allocator->done(lastExpectedParticle);
	}

	int particlesBeforeLoopOrEnd = 0;

	while(true) {
		T* particle = allocator->next();
		if(particle == lastExpectedParticle) break;
		allocator->done(particle);
		particlesBeforeLoopOrEnd++;
		if(rand() % 10 < 5) { // about half the particles will be updated.
			TEST_ASSERT_TRUE(updater->hasNext());
			updater->done(updater->next());
			if(rand() % 10 < 5) {
				living->next(); //we randomize some dangling living pointers, to make sure that they are reset anyway.
				//(Notice  that we do NOT call living-done() here.
			} else {
				living->done(living->next());
			}
		} else if(rand() % 10 < 5) {
			TEST_ASSERT_TRUE(updater->hasNext());
			updater->next(); //And some dangling update pointers.
		}
	}
	return particlesBeforeLoopOrEnd;
}



}



#endif /* PARTICLEPOOL_TEST_H_ */
