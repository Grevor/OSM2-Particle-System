/*
 * SizeInitializer.hpp
 *
 *  Created on: 9 maj 2014
 *      Author: Erik
 */

#ifndef SIZEINITIALIZER_HPP_
#define SIZEINITIALIZER_HPP_

#include "../ParticleInitializer.h"
#include "StandardParticle.hpp"

class SizeInitializer : public ParticleInitializer<StandardParticle> {
	Vector3f s;
public:
	SizeInitializer(Vector3f& size) {
		s = size;
	}

	void initParticle(StandardParticle* particle) {
		particle->render.size = s;
	}
};



#endif /* SIZEINITIALIZER_HPP_ */
