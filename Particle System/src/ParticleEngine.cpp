/*
 * ParticleEngine.cpp
 *
 *  Created on: 24 apr 2014
 *      Author: David
 */

#include "ParticleEngine.h"

#include <boost/phoenix/bind/bind_member_function.hpp>

ParticleEngine::ParticleEngine()
{
	//boost::asio::io_service::work work(ioService);
	workPool = new WorkPool();

	unsigned int max = boost::thread::hardware_concurrency();
	for (unsigned int threadId = 0; threadId < max; ++threadId) {
		threadpool.create_thread(boost::bind(&WorkPool::run, workPool));
	}
}

ParticleEngine::~ParticleEngine()
{
	// TODO Auto-generated destructor stub
}

void ParticleEngine::step() {

}

void ParticleEngine::update() {
	std::list<IterationUpdateable*>::iterator iter = particleSystems.begin();
	while(iter != particleSystems.end()) {
		(*iter)->step();
		workPool->postWork((void (*)())boost::bind(&IterationUpdateable::update, *iter));
		iter++;
	}
}

void ParticleEngine::addParticleSystem(IterationUpdateable* particleSystem) {
	if (particleSystem == NULL) {
		throw std::invalid_argument( "Can't add NULL to ParticleEngine." );
	}
	particleSystems.push_front(particleSystem);
}

void ParticleEngine::removeParticleSystem(IterationUpdateable* particleSystem) {
	if (particleSystem == NULL) {
			throw std::invalid_argument( "Can't remove NULL to ParticleEngine." );
	}
	particleSystems.remove(particleSystem);
}
