/*
 * Updater.hpp
 *
 *  Created on: 12 maj 2014
 *      Author: Erik
 */

#ifndef UPDATER_HPP_
#define UPDATER_HPP_

#include "Particle.hpp"
#include <ParticleUpdater.h>
#include "Camera.hpp"

class StandardUpdater : public ParticleUpdater<Particle> {
	Camera* cam;
	double lastStep, thisStep;

public:
	StandardUpdater(Camera* cam, double startTime) {
		this->cam = cam;
		lastStep = -DBL_MAX;
		thisStep = -DBL_MAX;
		setTime(startTime);
	}

	void setTime(double d) {
		if(lastStep == -DBL_MAX) {
			lastStep = d;
			thisStep = d;
		}
		else {
			lastStep = thisStep;
			thisStep = d;
		}
	}

	bool updateParticle(Particle* p) override {
		float delta = thisStep - lastStep;
		//vec3 cameraPosition = cam->getPosition();
		if(p->life > 0.0f){
			// Decrease life
			p->life -= delta;
			if (p->life > 0.0f){
				// Simulate simple physics : gravity only, no collisions
				p->speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
				p->pos += p->speed * (float)delta;
				return false;

			}
			else{
				// Particles that just died will be put at the end of the buffer in SortParticles();
				return true;
			}
		}
		return true;
	}
};

#endif /* UPDATER_HPP_ */
