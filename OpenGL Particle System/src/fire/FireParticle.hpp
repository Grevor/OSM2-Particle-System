/*
 * FireParticle.hpp
 *
 *  Created on: 28 maj 2014
 *      Author: Erik
 */

#ifndef FIREPARTICLE_HPP_
#define FIREPARTICLE_HPP_

#include "../Particle.hpp"
#include <glm/glm.hpp>

class FireParticle : public Particle {
public:
	float oscFreq;
	float oscRate;
	vec3 actualPos;
	float actualSize;
};


#endif /* FIREPARTICLE_HPP_ */
