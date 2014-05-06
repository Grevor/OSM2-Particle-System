/*
 * ConstantForceUpdater.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef CONSTANTFORCEUPDATER_HPP_
#define CONSTANTFORCEUPDATER_HPP_
#include "../ParticleUpdater.h"
#include "StandardParticle.hpp"
#include <Eigen/Eigen>

using namespace Eigen;
class ConstantForceUpdater : ParticleUpdater<StandardParticle> {
	Vector3f cForce;
public:
	ConstantForceUpdater(Vector3f cForce) {
		this->cForce = cForce;
	}

	bool updateParticle(StandardParticle* particle) {
		particle->vec.addForce(cForce);
		return false;
	}
};




#endif /* CONSTANTFORCEUPDATER_HPP_ */