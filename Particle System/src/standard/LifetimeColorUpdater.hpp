/*
 * LifetimeColorUpdater.hpp
 *
 *  Created on: 6 maj 2014
 *      Author: Erik
 */

#ifndef LIFETIMECOLORUPDATER_HPP_
#define LIFETIMECOLORUPDATER_HPP_
#include "ColorUpdater.hpp"

class LifetimeColorUpdater : ColorUpdater {
protected:
	float start, end;
public:
	LifetimeColorUpdater(Vector3f target, float start, float end) : ColorUpdater(target) {
		this->start = start;
		this->end = end;
	}

	virtual Vector3f getColor(StandardParticle* particle) override {
		if(start <= particle->lifetime.lifetimeElapsed()) {
			if(particle->lifetime.lifetimeElapsed() <= end) {
				float interpolation = (particle->lifetime.lifetimeElapsed() - start) / (end - start);
				return (1 - interpolation) * particle->render.color + interpolation * this->targetColor;

			} else {
				return this->targetColor;
			}
		}
		return particle->render.color;
	}
};


#endif /* LIFETIMECOLORUPDATER_HPP_ */
