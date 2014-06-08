/*
 * LifetimeUpdater.hpp
 *
 *  Created on: 5 maj 2014
 */

#ifndef LIFETIMEUPDATER_HPP_
#define LIFETIMEUPDATER_HPP_
#include "../ParticleUpdater.h"
#include "../ParticleInitializer.h"
#include "../utilities/Timer.h"

class LifetimeUpdater : public ParticleUpdater<StandardParticle>, public ParticleInitializer<StandardParticle> {
	Timer<float>* timer;

public:
	LifetimeUpdater(Timer<float>* timer) {
		this->timer = timer;
	}

	bool updateParticle(StandardParticle* particle) override {
		//float deltaTime = particle->lifetime.lifetimeElapsed();
		particle->lifetime.setElapsed(timer->getTime());
		//deltaTime = particle->lifetime.lifetimeElapsed - deltaTime;
		//particle->vec.updatePosAndVel(deltaTime);
		return false;
	}

	void initParticle(StandardParticle* particle) override {
		particle->lifetime.lifetimeStart = timer->getTime();
		particle->lifetime.lifetimeCurrent = timer->getTime();
	}
};


#endif /* LIFETIMEUPDATER_HPP_ */
