/*
 * LifetimeUpdater.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef LIFETIMEUPDATER_HPP_
#define LIFETIMEUPDATER_HPP_
#include "../ParticleUpdater.h"
#include "../Timer.h"

class LifetimeUpdater : public ParticleUpdater<StandardParticle> {
	Timer<float>* timer;

public:
	LifetimeUpdater(Timer<float>* timer) {
		this->timer = timer;
	}

	bool updateParticle(StandardParticle* particle) override {
		float deltaTime = particle->lifetime.lifetimeElapsed;
		particle->lifetime.setElapsed(timer->getTime());
		deltaTime = particle->lifetime.lifetimeElapsed - deltaTime;
		particle->vec.updatePosAndVel(deltaTime);
		return false;
	}
};


#endif /* LIFETIMEUPDATER_HPP_ */
