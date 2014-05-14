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
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/bind.hpp>

#include "IterationUpdateable.h"

class ParticleEngine: public Stepable {
	boost::thread_group threads;
	std::list<IterationUpdateable*> particleSystems;
	typename std::list<IterationUpdateable*>::iterator taskIter;
	boost::mutex taskMutex; //must be held to fetch a task
	boost::interprocess::interprocess_semaphore taskSemaphore;
	std::atomic<int> nThreads;
	std::atomic<int> nWorkingThreads;
	std::atomic<int> nTasks;
	bool isRunning;

public:
	ParticleEngine() : ParticleEngine(boost::thread::hardware_concurrency()) {}

	ParticleEngine(int numThreads) : taskSemaphore(0) {
		if (numThreads < 1)
			throw std::invalid_argument("Too few threads: " + numThreads);
		this->isRunning = true;
		this->nThreads = 0;
		this->nWorkingThreads = 0;
		this->nTasks = 0;
		for (int threadId = 0; threadId < numThreads; ++threadId) {
			threads.create_thread(boost::bind(&ParticleEngine::performTasks, this));
		}
	}

	virtual ~ParticleEngine() {
		this->stop();
	}

	void step() override {
		typename std::list<IterationUpdateable*>::iterator iter = particleSystems.begin();
		taskIter = std::list<IterationUpdateable*>::iterator(iter);
		int newTaskCount = 0;
		while (iter != particleSystems.end()) {
			(*iter)->step();
			iter++;
			newTaskCount++;
			taskSemaphore.post();
		}
		int prevNTasks = nTasks;
		std::atomic_compare_exchange_strong(&nTasks, &prevNTasks,
				prevNTasks + newTaskCount);
	}

	bool isStepComplete() override {
		return (nWorkingThreads == 0) && (nTasks == 0);
	}
	/**
	 * Blocking call which returns when the step is complete.
	 */
	void waitStepComplete() {
		//TODO: naive implementation, busy wait. improve this
		while (!isStepComplete()) {}
	}
	/**
	 * Adds a particle system to this particle engine.
	 * @param particleSystem
	 */
	void addParticleSystem(IterationUpdateable* particleSystem) {
		if (particleSystem == NULL) {
			throw std::invalid_argument("Can't add NULL to ParticleEngine.");
		}
		particleSystems.push_front(particleSystem);
	}
	/**
	 * Removes a particle system from this particle engine.
	 * @param particleSystem
	 */
	void removeParticleSystem(IterationUpdateable* particleSystem) {
		if (particleSystem == NULL) {
			throw std::invalid_argument("Can't remove NULL from ParticleEngine.");
		}
		particleSystems.remove(particleSystem);
	}

private:
	/**
	 * Entry point of worker threads. Threads calling this method be enslaved under
	 * the particle engine until it is stopped by any thread.
	 */
	void performTasks() {
		++nThreads;
		while (isRunning) {
			taskSemaphore.wait();
			if (!isRunning) {
				break;
			}

			++nWorkingThreads;

			taskMutex.lock();
			IterationUpdateable* task = *taskIter;
			taskIter++;
			taskMutex.unlock();

			task->update();

			--nWorkingThreads;

			--nTasks;
		}
		--nThreads;
	}
	/*
	 * Restarts a stopped WorkPool.
	 *
	void start();*/
	/**
	 * Stops this ParticleEngine, preventing additional work to be added
	 * and causing worker threads to return when there is no more
	 * work left in the pool.
	 */
	void stop() {
		this->isRunning = false;
		int nWaitingThreads = nThreads - nWorkingThreads;
		for (int i = 0; i < nWaitingThreads; ++i) {
			taskSemaphore.post();
		}
		threads.join_all();
	}
};

#endif /* PARTICLEENGINE_H_ */
