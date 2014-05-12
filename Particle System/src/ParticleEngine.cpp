/*
 * ParticleEngine.cpp
 *
 *  Created on: 24 apr 2014
 *      Author: David
 */

#include "ParticleEngine.h"

#include <boost/bind.hpp>

ParticleEngine::ParticleEngine() : ParticleEngine(boost::thread::hardware_concurrency()) {}

ParticleEngine::ParticleEngine(int numThreads) : taskSemaphore(0) {
	if (numThreads < 1)
		throw std::invalid_argument("Too few threads: " + numThreads);
	this->isRunning = true;
	this->nThreads = 0;
	this->nWorkingThreads = 0;
	this->nTasks = 0;
	for (unsigned int threadId = 0; threadId < numThreads; ++threadId) {
		threads.create_thread(boost::bind(&ParticleEngine::performTasks, this));
	}
}

ParticleEngine::~ParticleEngine() {
	this->stop();
	//delete threads;
	//delete particleSystems;
}

void ParticleEngine::step() {
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

bool ParticleEngine::isStepComplete() {
	return (nWorkingThreads == 0) && (nTasks == 0);
}

void ParticleEngine::waitStepComplete() {
	//TODO: naive implementation, busy wait. improve this
	while (!isStepComplete()) {}
}

void ParticleEngine::addParticleSystem(IterationUpdateable* particleSystem) {
	if (particleSystem == NULL) {
		throw std::invalid_argument("Can't add NULL to ParticleEngine.");
	}
	particleSystems.push_front(particleSystem);
}

void ParticleEngine::removeParticleSystem(IterationUpdateable* particleSystem) {
	if (particleSystem == NULL) {
		throw std::invalid_argument("Can't remove NULL from ParticleEngine.");
	}
	particleSystems.remove(particleSystem);
}

void ParticleEngine::performTasks() {
	/*int prevNThreads = nThreads;
	 std::atomic_compare_exchange_strong(&nThreads, &prevNThreads, prevNThreads+1);
	 int prevNWorkingThreads;
	 int prevNTasks;*/
	++nThreads;
	while (isRunning) {
		taskSemaphore.wait();
		if (!isRunning) {
			break;
		}

		//prevNWorkingThreads = nWorkingThreads;
		//std::atomic_compare_exchange_strong(&nWorkingThreads, &prevNWorkingThreads, prevNWorkingThreads+1);
		++nWorkingThreads;

		taskMutex.lock();
		IterationUpdateable* task = *taskIter;
		taskIter++;
		taskMutex.unlock();

		task->update();

		//prevNWorkingThreads = nWorkingThreads;
		//std::atomic_compare_exchange_strong(&nWorkingThreads, &prevNWorkingThreads, prevNWorkingThreads-1);
		--nWorkingThreads;

		//prevNTasks = nTasks;
		//std::atomic_compare_exchange_strong(&nTasks, &prevNTasks, prevNTasks+1);
		--nTasks;

	}
	//prevNThreads = nThreads;
	//std::atomic_compare_exchange_strong(&nThreads, &prevNThreads, prevNThreads-1);
	--nThreads;
}

/*void ParticleEngine::start() {
 if(!isRunning) {
 isRunning = true;
 //TODO: respawn threads
 }
 }
 */
void ParticleEngine::stop() {
	this->isRunning = false;
	int nWaitingThreads = nThreads - nWorkingThreads;
	for (int i = 0; i < nWaitingThreads; ++i) {
		taskSemaphore.post();
	}
	threads.join_all();
}
