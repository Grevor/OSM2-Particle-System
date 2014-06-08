/*
 * ColorUpdater.hpp
 *
 *  Created on: 6 maj 2014
 */

#ifndef COLORUPDATER_HPP_
#define COLORUPDATER_HPP_
#include "../ParticleUpdater.h"
#include "StandardParticle.hpp"
#include <Eigen/Eigen>

/**
 * Updater which enforces a certain color to a particle.
 */
class ColorUpdater : public ParticleUpdater<StandardParticle> {
protected:
	Vector3f targetColor;
public:

	ColorUpdater(Vector3f& targetColor) {
		this->targetColor = targetColor;
	}

	virtual Vector3f getColor(StandardParticle* particle) {
		return targetColor;
	}

	bool updateParticle(StandardParticle* particle) {
		particle->render.color = getColor(particle);
		return false;
	}

	~ColorUpdater() {}
};



#endif /* COLORUPDATER_HPP_ */
