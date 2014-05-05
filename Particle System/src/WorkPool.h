/*
 * WorkPool.h
 *
 *  Created on: 25 apr 2014
 *      Author: David
 */

#ifndef WORKPOOL_H_
#define WORKPOOL_H_

#include <boost/signals2/mutex.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

class WorkPool {
	std::list<void (*)(void)> workFunctions;
	boost::mutex workFuctionsMutex;
	boost::interprocess::interprocess_semaphore workSemaphore;
	int nWorkingThreads;
public:
	WorkPool();
	virtual ~WorkPool();
	/**
	 * Entry point of worker thread.
	 */
	void run();
	/**
	 * Restarts a stopped WorkPool.
	 */
	void start();
	/**
	 * Stops this WorkPool, preventing additional work to be added
	 * and causing worker threads to return when there is no more
	 * work left in the pool.
	 */
	void stop();
	/**
	 * Checks if the WorkPool is empty.
	 * @return true if empty, else false.
	 */
	bool isEmpty();
	/**
	 * Blocking call that returns when this WorkPool is empty.
	 */
	void waitUntilEmpty();
	/**
	 * Posts a work to this WorkPool.
	 * @param work is a new work to perform.
	 */
	void postWork(void (*workFunction)(void));
};

#endif /* WORKPOOL_H_ */
