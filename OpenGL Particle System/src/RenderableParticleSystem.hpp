/*
 * RenderableParticleSystem.hpp
 *
 *  Created on: 20 maj 2014
 *      Author: David
 */

#ifndef RENDERABLEPARTICLESYSTEM_HPP_
#define RENDERABLEPARTICLESYSTEM_HPP_

#include <GL/gl.h>
#include "Particle.hpp"
#include "ParticleSystem.hpp"
#include "ParticleMaterial.hpp"

class RenderableParticleSystem {
	ParticleSystem<Particle>* particleSystem;
	//ParticleMaterial* material;
	int materialIndex;
public:
	RenderableParticleSystem(ParticleSystem<Particle>* particleSystem, int materialIndex) {
		this->particleSystem = particleSystem;
		this->materialIndex = materialIndex;
	}

	int getMaterialIndex() {
		return materialIndex;
	}

	ParticleSystem<Particle>* getParticleSystem() {
		return particleSystem;
	}
};

#endif /* RENDERABLEPARTICLESYSTEM_HPP_ */
