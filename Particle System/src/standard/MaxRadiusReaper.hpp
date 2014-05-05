/*
 * MaxRadiusReaper.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef MAXRADIUSREAPER_HPP_
#define MAXRADIUSREAPER_HPP_
#include "../Reaper.h"
#include "StandardParticle.hpp"
#include <Eigen/Eigen>
#include "../ConstantCurve.h"

using namespace Curves;
class MaxRadiusReaper : public Reaper<StandardParticle> {
	Curve<float, float>* maxRadius;
	ControlPoint* origin;
	float input;
	bool fade;
public:

	MaxRadiusReaper(float maxRadius, ControlPoint* origin, bool fadeOut) {
		this->maxRadius = new ConstantCurve(maxRadius);
		this->input = 0;
		this->origin = origin;
		fade = fadeOut;
	}

	MaxRadiusReaper(Curve<float,float>* curve, float initialInput, ControlPoint* origin, bool fadeOut) {
		this->maxRadius = curve;
		setInput(initialInput);
		this->origin = origin;
		fade = fadeOut;
	}

	bool isDead(StandardParticle* particle) override {
		Vector3f particlePositionRelatedToOrigin = particle->vec.pos;
		if(!particle->isBoundToOrigin() && origin != NULL) particlePositionRelatedToOrigin -= origin->position();
		bool result = particlePositionRelatedToOrigin.norm() > maxRadius->getValue(input);
		//If the particle is outside the bounds and fade is enabled
		if (result && fade) {
			//Start the fade-out if not already started.
			if(!particle->lifetime.fadingOut()) {
				particle->lifetime.setElapsed(particle->lifetime.fadeOutBegin);
			}
			return false;
		}
		return result;
	}

	/**
	 * Sets the input parameter used when sampling the max-distance curve.
	 * @param newInput The new input to use to the curve.
	 */
	void setInput(float newInput) { this->input = newInput; }

	/**
	 * Sets the origin used by this reaper. Do note that changing this may cause strange behaviors.
	 * @param newOrigin The new Origin.
	 */
	void setOrigin(ControlPoint* newOrigin) { this->origin = newOrigin; }

	virtual ~MaxRadiusReaper() {
		if(this->maxRadius != NULL) delete maxRadius;
	}
};




#endif /* MAXRADIUSREAPER_HPP_ */
