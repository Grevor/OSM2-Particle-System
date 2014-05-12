/*
 * Initializer.hpp
 *
 *  Created on: 12 maj 2014
 *      Author: Erik
 */

#ifndef INITIALIZER_HPP_
#define INITIALIZER_HPP_

#include "ParticleInitializer.h"
#include "Particle.hpp"
#include <GL/glext.h>
#include <glm/gtc/random.hpp>

using namespace glm;

class StandardParticleInitializer : public ParticleInitializer<Particle> {
	vec3 position;
public:
	StandardParticleInitializer(glm::vec3& pos) {
		position = pos;
	}

	void initParticle(Particle* particle) override {
		particle->life = 5.0f; // This particle will live 5 seconds.
				particle->pos = position;

				float spread = 1.5f;
				glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
				// Very bad way to generate a random direction;
				// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
				// combined with some user-controlled parameters (main direction, spread, etc)
				glm::vec3 randomdir = glm::sphericalRand(1);

				particle->speed = maindir + randomdir*spread;

				// Very bad way to generate a random color
				particle->r = rand() % 256;
				particle->g = rand() % 256;
				particle->b = rand() % 256;
				particle->a = (rand() % 256) / 3;

				particle->size = (rand()%1000)/2000.0f + 0.1f;
	}
};


#endif /* INITIALIZER_HPP_ */
