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
#include <boost/bind.hpp>

#include "IterationUpdateable.h"

/**
 * A ParticleEngine supplies threads to concurrently update multiple particle systems.
 */
class ParticleEngine: public Stepable {
	boost::thread_group threads;
	std::list<IterationUpdateable*> particleSystems;
	std::list<IterationUpdateable*> startedTasks;
	typename std::list<IterationUpdateable*>::iterator newTaskIter;
	boost::mutex mutex; //monitor mutex
	boost::condition_variable newTasksCV;
	boost::condition_variable taskCompletionCV;
	std::atomic<int> nThreads;
	std::atomic<int> nUninititializedTasks;
	bool isRunning;

public:
	ParticleEngine() : ParticleEngine(boost::thread::hardware_concurrency()) {}

	/**
	 * Constructor
	 * @param numThreads is the number of threads which this ParticleEngine uses
	 * to update the particle systems.
	 */
	ParticleEngine(int numThreads)
	{
		if (numThreads < 1)
			throw std::invalid_argument("Too few threads: " + numThreads);
		this->isRunning = true;
		this->nThreads = numThreads;
		this->nUninititializedTasks = 0;
		for (int threadId = 0; threadId < numThreads; ++threadId) {
			threads.create_thread(boost::bind(&ParticleEngine::performTasks, this));
		}
	}

	virtual ~ParticleEngine() {
		this->stop();
	}

	void step() override {
		boost::mutex::scoped_lock lock(mutex);
		waitStepComplete(&lock);
		newTaskIter = particleSystems.begin();
		nUninititializedTasks = particleSystems.size();
		newTasksCV.notify_all();
	}

	bool isStepComplete() override {
		return nUninititializedTasks == 0 && startedTasks.empty();
	}
	/**
	 * Blocking call which returns when the step is complete.
	 */
	void waitStepComplete() {
		boost::mutex::scoped_lock lock(mutex);
		if (isStepComplete())
			return;
		taskCompletionCV.wait(lock);
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
		boost::mutex::scoped_lock lock(mutex);
		waitStepComplete(&lock);
		particleSystems.remove(particleSystem);
	}

private:
	void waitStepComplete(boost::mutex::scoped_lock* lock) {
		if (nUninititializedTasks == 0)
			return;
		taskCompletionCV.wait(*lock);
	}
	/**
	 * Entry point of worker threads. Threads calling this method be enslaved under
	 * the particle engine until it is stopped by any thread.
	 */
	void performTasks() {
		boost::mutex::scoped_lock lock(mutex);
		while (isRunning) {
			if (isStepComplete()) {
				newTasksCV.wait(lock);
			}
			//Pick up uninitialized task if such exist
			IterationUpdateable* task = NULL;
			while (newTaskIter != particleSystems.end()) {
				nUninititializedTasks--;
				if ((*newTaskIter)->isAlive()) {
					task = *newTaskIter;
					++newTaskIter;
					task->step();
					startedTasks.push_back(task);
					lock.unlock();
					task->update();
					lock.lock();
				}
				else {
					particleSystems.erase(newTaskIter++);
				}
			}
			lock.unlock();
			//Help to finish incomplete tasks
			while (!startedTasks.empty()) {
				if (mutex.try_lock()) {
					task = NULL;
					while (!startedTasks.empty()) {
						task = startedTasks.front();
						startedTasks.pop_front();
						if (!task->isStepComplete()) {
							startedTasks.push_back(task);
							break;
						}
						if (startedTasks.empty())
							taskCompletionCV.notify_all();
					}
					mutex.unlock();
					if (task != NULL) {
						task->update();
					}
				}
			}
			lock.lock(); //lock for next loop iteration
		}
		--nThreads;
	}
	/**
	 * Stops this ParticleEngine, preventing additional work to be added
	 * and causing worker threads to return when there is no more
	 * work left in the pool.
	 */
	void stop() {
		boost::mutex::scoped_lock lock(mutex);
		this->isRunning = false;
		newTasksCV.notify_all();
		lock.unlock();
		threads.join_all();
	}
};

#endif /* PARTICLEENGINE_H_ */
