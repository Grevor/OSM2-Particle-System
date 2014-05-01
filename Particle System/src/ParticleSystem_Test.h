/*
 * ParticleSystem_Test.h
 *
 *  Created on: 29 apr 2014
 *      Author: Erik
 */

#ifndef PARTICLESYSTEM_TEST_H_
#define PARTICLESYSTEM_TEST_H_
#include <stdlib.h>
#include <atomic>
#include "ParticleIterator.h"
#include "ParticleSystem.h"
#include "ParticlePool.h"
#include "DebugHelp.h"
#include "Curve.h"
#include "ParticleUpdater.h"

using namespace Curves;

#define VARIADIC_POOL_SIZE_SIZE 1 << 16
#define NUM_THREADS 10

template<typename T> struct particleTest_t {
	ParticleSystem<T>* system;
	ParticleIterator<T>* iter;
	Curve<long,long>* spawnCurve;
	std::atomic<int64_t> atomicCounter;
	bool (*validationFunction)(T* particle, int currentStep);
	T expectedVal;
	int poolSize;
	int iterations;
};

//Denotes a thread which is living.
template<typename T> void* livingThread(void* data) {
	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;
	ParticleIterator<T>* iter = pData->iter;
	while(iter->hasNext()) {
		T* particle = iter->next();
		if(particle == NULL) return NULL;
		TEST_ASSERT_TRUE(*particle == pData->expectedVal);
		waitFreeUpdate(&pData->atomicCounter, 1);
	}
	return NULL;
}

template<typename T> void* updateThread(void* data) {
	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;
	sleep(rand()%10);
	pData->system->update();
	return NULL;
}

void waitFreeUpdate(std::atomic<int64_t>* var, int64_t delta) {
	while(true) {
		int64_t expected = *var;
		int64_t desired = expected - delta;
		if(std::atomic_compare_exchange_strong(var,&expected,desired)) break;
	}
}


void spawnWorkerThreads(int num, void* (*fn)(void*), void* data) {
	pthread_t thread;
	for(int i = 0; i < num; i++) {
		pthread_create(&thread, NULL, fn, data);
	}
}

/**
 * Tests the specified system, invoking all functions on it.
 * @param system
 * @param particleValidator
 * @param spawnCurve
 * @param pool
 * @param poolSize
 * @param iterations
 */
template<typename T> void testParticleSystem(
		ParticleSystem<T>* system,
		bool (*particleValidator)(T* particle, int iteration),
		Curve<long,long>* spawnCurve,
		ParticlePool<T>* pool,
		int poolSize,
		int iterations) {
	struct particleTest_t<T> testData;
	testData.system = system;
	testData.validationFunction = particleValidator;
	testData.atomicCounter = 0;
	testData.iterations = iterations;
	testData.poolSize = poolSize;

	ensureCorrectInitializationAndLivingIterator_multiThreadedUpdate<T>(&testData);
	ensureSingularityAndValidity_multiThreadedUpdate(&testData);
}

/**
 * Checks that the specified value is not in the array.
 * @param array - The array
 * @param size - The size
 * @param type The object to check for.
 * @return
 */
template<typename T> bool notInArray(T* array, int size, T type) {
	for(int i = 0; i < size; i++) {
		if(array[i] == type) return false;
	}
	return true;
}

/**
 * Ensures correct initialization and that the living particle-iterator works as specified in the interface. This test may not apply to all systems.
 *
 * [NOTICE] This test assumes that no particle dies during the specified number of iterations.
 *
 * @param data - Pointer to a struct particleTest_t<T> containing all information about the test.
 */
template<typename T> void* ensureCorrectInitializationAndLivingIterator_multiThreadedUpdate(void* data) {
	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;

	int64_t numParticles = 0;
	int64_t numParticlesThisIteration = 0;

	for(int i = 0; pData->iterations; i++) {
		pData->system->step();
		numParticles += pData->spawnCurve->getValue(i+1);
		numParticlesThisIteration = 0;
		sleep(rand() % 10);
		ParticleIterator<T>* iter = pData->system->getLivingIterator();
		TEST_ASSERT_TRUE(iter != NULL);

		while(iter->next() != NULL) {
			numParticlesThisIteration++;
		}

		TEST_ASSERT_TRUE(numParticlesThisIteration == numParticles);
		delete iter;
	}
}

/**
 * Ensures that no value is read twice in one iteration, and that no value is updated twice.
 *
 * [NOTICE] This test assumes that all particles are the same at a specified iterations, unless some smart stuff is done in the validation function to alleviate this.
 *
 * @param data - Pointer to a struct particleTest_t<T> containing all data for the test.
 */
template<typename T> void* ensureSingularityAndValidity_multiThreadedUpdate(void* data) {
	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;

	T** readParticles;
	int readSize;
	if(pData->poolSize >= 0) readSize = pData->poolSize;
	else readSize = VARIADIC_POOL_SIZE_SIZE;

	readParticles = (T**)calloc(readSize,sizeof(T*));
	int currentReadPos = 0;

	for(int i = 0; i < pData->iterations; i++) {
		currentReadPos = 0;
		pData->system->step();
		spawnWorkerThreads(NUM_THREADS,&updateThread<T>,data);
		ParticleIterator<T>* iter = pData->system->getLivingIterator();

		while(iter->hasNext()) {
			T* particle = iter->next();
			if(particle == NULL) break;
			TEST_ASSERT_TRUE(pData->validationFunction(particle, i));
			TEST_ASSERT_TRUE(notInArray<T*>(readParticles, currentReadPos, particle));
			if(currentReadPos <= readSize) {
				readParticles[currentReadPos] = particle;
				currentReadPos++;
			}
			iter->done(particle);
		}
		delete iter;
	}
	return NULL;
}

#endif /* PARTICLESYSTEM_TEST_H_ */
