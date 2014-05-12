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
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};



#endif /* PARTICLE_HPP_ */
