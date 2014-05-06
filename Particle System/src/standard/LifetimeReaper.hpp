/*
 * LifetimeReaper.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef LIFETIMEREAPER_HPP_
#define LIFETIMEREAPER_HPP_

#include "../Reaper.h"

/**
 * Class defining a very standard reaper which is enacted when a standard-particle is found to have exceeded its lifetime.
 */
class LifetimeReaper : public Reaper<StandardParticle> {

public:
	bool isDead(StandardParticle* particle) override {
		return particle->lifetime.lifetimeCurrent >= particle->lifetime.lifetimeStart + particle->lifetime.lifetime;
	}
};



#endif /* LIFETIMEREAPER_HPP_ */
