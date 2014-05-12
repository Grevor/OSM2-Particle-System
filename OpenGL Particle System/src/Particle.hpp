/*
 * Particle.hpp
 *
 *  Created on: 12 maj 2014
 *      Author: Erik
 */

#ifndef PARTICLE_HPP_
#define PARTICLE_HPP_

#include <glm/glm.hpp>

using namespace glm;

// CPU representation of a particle
struct Particle{
	glm::vec3 pos, speed;
	unsigned char r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
};



#endif /* PARTICLE_HPP_ */
