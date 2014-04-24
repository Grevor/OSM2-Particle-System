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
	boost::asio::io_service::work work(ioService);

	unsigned int max = boost::thread::hardware_concurrency();
	for (unsigned int threadId = 0; threadId < max; ++threadId) {
		threadpool.create_thread(
				boost::bind(&boost::asio::io_service::run, &ioService));
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
		ioService.post(boost::bind(&IterationUpdateable::update, *iter));
		iter++;
	}
	threadpool.join_all();
}

void ParticleEngine::addParticleSystem(IterationUpdateable* particleSystem) {

}

void ParticleEngine::removeParticleSystem(IterationUpdateable* particleSystem) {

}
