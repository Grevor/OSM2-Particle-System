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
	std::list<IterationUpdateable*> startedTasks;
	typename std::list<IterationUpdateable*>::iterator newTaskIter;
	boost::mutex mutex; //monitor mutex
	boost::condition_variable newTasksCV;
	boost::condition_variable taskCompletionCV;
	//boost::interprocess::interprocess_semaphore taskSemaphore;
	std::atomic<int> nThreads;
	std::atomic<int> nUninititializedTasks;
	bool isRunning;

public:
	ParticleEngine() : ParticleEngine(boost::thread::hardware_concurrency()) {}

	ParticleEngine(int numThreads)
		//taskSemaphore(0)
	{
		if (numThreads < 1)
			throw std::invalid_argument("Too few threads: " + numThreads);
		this->isRunning = true;
		this->nThreads = numThreads;
		//this->nWorkingThreads = numThreads;
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
		/*taskMutex.lock();
		typename std::list<IterationUpdateable*>::iterator iter = particleSystems.begin();
		newTaskIter = std::list<IterationUpdateable*>::iterator(iter);
		int newTaskCount = 0;
		while (iter != particleSystems.end()) {
			(*iter)->step();
			iter++;
			newTaskCount++;
			taskSemaphore.post();
		}
		nTasks += newTaskCount;*/
	}

	bool isStepComplete() override {
		return nUninititializedTasks == 0 && startedTasks.empty();
	}
	/**
	 * Blocking call which returns when the step is complete.
	 */
	void waitStepComplete() {
		//TODO: naive implementation, busy wait. improve this
		//while (!isStepComplete()) {}
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

			/*taskSemaphore.wait();
			if (!isRunning) {
				break;
			}

			++nWorkingThreads;

			taskMutex.lock();
			IterationUpdateable* task = *newTaskIter;
			newTaskIter++;
			taskMutex.unlock();

			task->update();

			--nWorkingThreads;

			--nTasks;*/
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
		/*int nWaitingThreads = nThreads - nWorkingThreads;
		for (int i = 0; i < nWaitingThreads; ++i) {
			taskSemaphore.post();
		}*/
		threads.join_all();
	}
};

#endif /* PARTICLEENGINE_H_ */
