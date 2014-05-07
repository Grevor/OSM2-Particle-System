/*
 * PhysicsUpdater.hpp
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef PHYSICSUPDATER_HPP_
#define PHYSICSUPDATER_HPP_

#include "../ParticleUpdater.h"

#include "StandardParticle.hpp"

class PhysicsUpdater : public ParticleUpdater<StandardParticle> {
public:
	PhysicsUpdater() {}

	bool updateParticle(StandardParticle* particle) {
		particle->vec.updatePosAndVel(1);
		return false;
	}
};



#endif /* PHYSICSUPDATER_HPP_ */
