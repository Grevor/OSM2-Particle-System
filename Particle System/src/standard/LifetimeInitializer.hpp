/*
 * LifetimeInitializer.hpp
 *
 *  Created on: 7 maj 2014
 */

#ifndef LIFETIMEINITIALIZER_HPP_
#define LIFETIMEINITIALIZER_HPP_

#include "../ParticleInitializer.h"
#include "StandardParticle.hpp"

class LifetimeInitializer : public ParticleInitializer<StandardParticle> {
	float lifetime;
public:
	LifetimeInitializer(float lifetime) {
		this->lifetime = lifetime;
	}

	void initParticle(StandardParticle* particle) override {
		particle->lifetime.lifetime = lifetime;
	}
};




#endif /* LIFETIMEINITIALIZER_HPP_ */
