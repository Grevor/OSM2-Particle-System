/*
 * ParticleEngine.h
 *
 *  Created on: 24 apr 2014
 *      Author: David
 */

#ifndef PARTICLEENGINE_H_
#define PARTICLEENGINE_H_

#include <list>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include <IterationUpdateable.h>

class ParticleEngine : IterationUpdateable<void> {
	std::list<IterationUpdateable*> particleSystems;
	boost::asio::io_service ioService;
	boost::thread_group threadpool;
public:
	ParticleEngine();
	virtual ~ParticleEngine();
	void step() override;
	void update() override;
	void addParticleSystem(IterationUpdateable* particleSystem);
	void removeParticleSystem(IterationUpdateable* particleSystem);
};

#endif /* PARTICLEENGINE_H_ */
