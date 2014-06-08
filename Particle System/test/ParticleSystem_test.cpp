/*
 * ParticleSystem_test.h
 *
 *  Created on: 29 apr 2014
 *
 *  [THIS IS NOT AN AUTOMATIC TEST]
 *  The following file contains no cool framework or otherwise. It simply contains methods for testing ParticleSystems.
 *  The general promise of these functions is that if they do not generate a SIGINT through assert(), they pass.
 *  Please note the comments of the corresponding test-functions when using them.
 *
 *  These tests work only under cygwin.
 */

#ifndef PARTICLESYSTEM_TEST_H_
#define PARTICLESYSTEM_TEST_H_
#include <stdlib.h>
#include <atomic>
#include "../src/ParticleIterator.h"
#include "../src/ParticleSystem.h"
#include "../src/ParticlePool.h"
#include "../src/DebugHelp.h"
#include "../src/Curve.h"
#include "../src/ParticleUpdater.h"

using namespace Curves;

#define VARIADIC_POOL_SIZE_SIZE 1 << 16
#define NUM_THREADS 1

template<typename T> struct particleTest_t {
	ParticleSystem<T>* system;
	ParticleIterator<T>* iter;
	Curve<long,long>* spawnCurve;
	std::atomic<int64_t> atomicCounter;
	bool (*validationFunction)(T* particle, int currentStep);
	T expectedVal;
	int poolSize;
	int iterations;
	bool threadsShouldSleep;
};

//Denotes a thread which is living.
template<typename T> void* livingThread(void* data) {
	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;
	ParticleIterator<T>* iter = pData->iter;
	while(iter->hasNext()) {
		T* particle = iter->next();
		if(particle == NULL) return NULL;
		TEST_ASSERT_TRUE(*particle == pData->expectedVal);
		lockFreeUpdate(&pData->atomicCounter, 1);
	}
	return NULL;
}

template<typename T> int64_t systemParticles(ParticleSystem<T>* system) {
	int64_t num = 0;
	ParticleIterator<T>* iter = system->getLivingParticles();
	T* p;
	while(true) {
		p = iter->next();
		if(p == NULL) break;
		num++;
		iter->done(p);
	}
	delete iter;
	return num;
}

//entry point for an update thread. Has a built-in sleep before starting operations to test certain cases.
template<typename T> void* updateThread(void* data) {
	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;
	if(pData->threadsShouldSleep) usleep(rand()%1000000);
	pData->system->update();
	return NULL;
}

/**
 * Updates an atomic int as desired.
 * @param var Pointer to atomic variable.
 * @param delta Difference.
 */
void lockFreeUpdate(std::atomic<int64_t>* var, int64_t delta) {
	while(true) {
		int64_t expected = *var;
		int64_t desired = expected - delta;
		if(std::atomic_compare_exchange_strong(var,&expected,desired)) break;
	}
}

/**
 * Spawns the desired number of threads.
 * @param num Amount od threads
 * @param fn The function to call.
 * @param data The function argument.
 */
pthread_t* spawnWorkerThreads(int num, void* (*fn)(void*), void* data) {
	pthread_t* threads = (pthread_t*) malloc(num * sizeof(pthread_t));
	for(int i = 0; i < num; i++) {
		pthread_create(&threads[i], NULL, fn, data);
	}
	return threads;
}


/**
 * Checks that the specified value is not in the array.
 * @param array - The array
 * @param size - The size
 * @param type The object to check for.
 * @return
 * True if the value is not in the array, else false.
 */
template<typename T> bool notInArray(T* array, int size, T type) {
	for(int i = 0; i < size; i++) {
		if(array[i] == type) return false;
	}
	return true;
}

void joinThreads(pthread_t* threads, int num) {
	for(; num > 0; num--) {
		pthread_join(threads[num - 1], NULL);
	}
	free(threads);
}

/**
 * Ensures correct initialization and that the living particle-iterator works as specified in the interface.
 * This test may not apply to all systems.
 *
 * [NOTICE] This test assumes that no particle dies during the specified number of iterations, and that the pool will not become full.
 *
 * @param data - Pointer to a struct particleTest_t<T> containing all information about the test.
 */
template<typename T> void* ensureCorrectInitializationAndLivingIterator_multiThreadedUpdate(void* data) {
	printf("Testing iterator behaviors and correct initialization and spawning with multi-threaded update running.\n");

	struct particleTest_t<T>* pData = (struct particleTest_t<T>*)data;

	int64_t numParticles = 0;
	int64_t lastNumParticles = 0;
	int64_t numParticlesThisIteration = 0;
	pthread_t* threads = NULL;

	for(int i = 0; i < pData->iterations; i++) {
		if(threads != NULL) joinThreads(threads, NUM_THREADS);
		TEST_ASSERT_EQUAL(numParticles, systemParticles(pData->system));

		pData->system->step();
		threads = spawnWorkerThreads(10,(updateThread<T>),data);
		lastNumParticles = numParticles;
		numParticles += pData->spawnCurve->getValue(i+1);
		numParticlesThisIteration = 0;
		usleep(rand() % 1000);
		ParticleIterator<T>* iter = pData->system->getLivingParticles();
		TEST_ASSERT_TRUE(iter != NULL);

		T* part;
		while(true) {
			part = iter->next();
			if(part == NULL) break;
			numParticlesThisIteration++;
			iter->done(part);
		}

		printf("Expected range: [%ld, %ld], particles: %ld\n", lastNumParticles, numParticles, numParticlesThisIteration);
		TEST_ASSERT_TRUE(lastNumParticles <= numParticlesThisIteration && numParticlesThisIteration <= numParticles);
		delete iter;
		printf("Ended iteration %d with valid values.\n", i + 1);
	}

	printf("Test succeeded.\n");
	return NULL;
}

/**
 * Ensures that no value is read twice in one iteration, and that no value is updated twice.
 *
 * [NOTICE] This test assumes that all particles are the same at a specified iterations, unless some smart stuff is done in the validation function to alleviate this.
 *
 * @param data - Pointer to a struct particleTest_t<T> containing all data for the test.
 */
template<typename T> void* ensureSingularityAndValidity_multiThreadedUpdate(void* data) {
	printf("Testing living iterator (no repeats) and particle validity.\n");
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
		ParticleIterator<T>* iter = pData->system->getLivingParticles();

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
	printf("Test succeeded.\n");
	return NULL;
}

template<typename T> void testReset() {

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
	testData.spawnCurve = spawnCurve;
	testData.threadsShouldSleep = true;

	srand(time(NULL));

	testAlive(system);
	system->reset();
	ensureCorrectInitializationAndLivingIterator_multiThreadedUpdate<T>((void*)&testData);
	system->reset();
	ensureSingularityAndValidity_multiThreadedUpdate<T>((void*)&testData);
	//system->reset();

	sleep(3);
}

/**
 * Tests the setAlive and isAlive functions.
 * @param system The system to test.
 */
template<typename T> void testAlive(ParticleSystem<T>* system) {
	//Tests persistent state after isAlive() call.
	bool state = system->isAlive();
	TEST_ASSERT_EQUAL(state, system->isAlive());

	//Tests simple case
	system->setAlive(false);
	TEST_ASSERT_FALSE(system->isAlive());
	system->setAlive(true);
	TEST_ASSERT_TRUE(system->isAlive());

	//Tests multiple of same. (and re-enabling/disabling)
	system->setAlive(false);
	system->setAlive(false);
	TEST_ASSERT_FALSE(system->isAlive());
	system->setAlive(true);
	system->setAlive(true);
	TEST_ASSERT_TRUE(system->isAlive());
}

#endif /* PARTICLESYSTEM_TEST_H_ */
