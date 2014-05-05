/*
 * ParticleEngine.h
 *
 *  Created on: 24 apr 2014
 *      Author: David
 */

#ifndef PARTICLEENGINE_H_
#define PARTICLEENGINE_H_

#include <list>
#include <atomic>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <IterationUpdateable.h>

class ParticleEngine: Stepable {
	boost::thread_group threads;
	std::list<IterationUpdateable*> particleSystems;
	std::list<IterationUpdateable*>::iterator taskIter;
	boost::mutex taskMutex; //must be held to fetch a task
	boost::interprocess::interprocess_semaphore taskSemaphore;
	std::atomic<int> nThreads;
	std::atomic<int> nWorkingThreads;
	std::atomic<int> nTasks;
	bool isRunning;
public:
	ParticleEngine();
	ParticleEngine(int numThreads);
	virtual ~ParticleEngine();
	void step() override;
	bool isStepComplete() override;
	/**
	 * Blocking call which returns when the step is complete.
	 */
	void waitStepComplete();
	/**
	 * Adds a particle system to this particle engine.
	 * @param particleSystem
	 */
	void addParticleSystem(IterationUpdateable* particleSystem);
	/**
	 * Removes a particle system from this particle engine.
	 * @param particleSystem
	 */
	void removeParticleSystem(IterationUpdateable* particleSystem);
private:
	/**
	 * Entry point of worker threads. Threads calling this method be enslaved under
	 * the particle engine until it is stopped by any thread.
	 */
	void performTasks();
	/*
	 * Restarts a stopped WorkPool.
	 *
	void start();*/
	/**
	 * Stops this ParticleEngine, preventing additional work to be added
	 * and causing worker threads to return when there is no more
	 * work left in the pool.
	 */
	void stop();
};

#endif /* PARTICLEENGINE_H_ */
