/*
 * OffsetInitializer.hpp
 *
 *  Created on: 5 maj 2014
 */

#ifndef OFFSETINITIALIZER_HPP_
#define OFFSETINITIALIZER_HPP_

#include <Eigen/Eigen>
#include "StandardParticle.hpp"
#include "../ParticleInitializer.h"

class OffsetInitializer : public ParticleInitializer<StandardParticle> {
	const Vector3f* offsetVector;
public:
	OffsetInitializer(const Vector3f* offsetObject) {
		offsetVector = offsetObject;
	}

	void initParticle(StandardParticle* particle) override {
		particle->vec.pos += *offsetVector;
	}
};



#endif /* OFFSETINITIALIZER_HPP_ */
