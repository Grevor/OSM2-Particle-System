/*
 * ExplosionVelocityInitializer.hpp
 *
 *  Created on: 7 maj 2014
 */

#ifndef EXPLOSIONVELOCITYINITIALIZER_HPP_
#define EXPLOSIONVELOCITYINITIALIZER_HPP_

#include "../ParticleInitializer.h"
#include "StandardParticle.hpp"

class ExplosionVelocityInitializer : public ParticleInitializer<StandardParticle> {
	Vector3f origin;
	float vel;
public:
	ExplosionVelocityInitializer(Vector3f& origin, float velocity) {
		this->origin = origin;
		vel = velocity;
	}

	void initParticle(StandardParticle* particle) override {
		Vector3f norm = particle->vec.pos - origin;
		norm.normalize();
		particle->vec.vel = norm * vel;
	}
};



#endif /* EXPLOSIONVELOCITYINITIALIZER_HPP_ */
