/*
 * OriginDistanceColorUpdater.hpp
 *
 *  Created on: 6 maj 2014
 */

#ifndef ORIGINDISTANCECOLORUPDATER_HPP_
#define ORIGINDISTANCECOLORUPDATER_HPP_
#include "LifetimeColorUpdater.hpp"

/**
 * Updater which interpolates the color of a particle depending on its distance from the origin.
 */
class OriginDistanceColorUpdater : public LifetimeColorUpdater {
public:
	/**
	 * Creates a new Updater which changes a particles color to the specified target color as it approaches a certain
	 * length from the origin.
	 *
	 * [NOTICE] This updater currently does not handle cases when particles "flutter about" randomly.
	 * This can be fixed easily, if desired.
	 * @param targetColor The color which the particle will have at "max" length.
	 * @param start The start of the interval in which the interpolation will occur.
	 * @param end The end of the interval in which the interpolation will occur.
	 */
	OriginDistanceColorUpdater(Vector3f& targetColor, float start, float end) :
		LifetimeColorUpdater(targetColor, start, end) { }

	Vector3f getColor(StandardParticle* particle) override {
		float distToOrigin = particle->vec.pos.norm();
		if(start <= distToOrigin) {
			if(distToOrigin <= end) {
				float interpolator = (distToOrigin - this->start) / (end - start);
				return (1-interpolator) * particle->render.color + interpolator * this->targetColor;
			} else {
				return this->targetColor;
			}
		}
		return particle->render.color;
	}
};


#endif /* ORIGINDISTANCECOLORUPDATER_HPP_ */
