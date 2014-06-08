/*
 * RadiusUpdater.hpp
 *
 *  Created on: 5 maj 2014
 */

#ifndef RADIUSUPDATER_HPP_
#define RADIUSUPDATER_HPP_
#include "../ParticleUpdater.h"
#include "StandardParticle.hpp"
#include <Eigen/Eigen>
#include "../curves/Curve.h"
#include "../curves/LinearCurve.hpp"

class RadiusUpdater : public ParticleUpdater<StandardParticle> {
	Curve<float,float>* radiusCurve;
	float start,end;
public:
	RadiusUpdater(float startTime, float endTime, float startVal, float endVal) {
		init(new LinearCurve<float>((endVal - startVal)/(endTime - startTime), startVal) ,startTime, endTime);
	}

	virtual ~RadiusUpdater() {
		if(radiusCurve != NULL) delete radiusCurve;
	}

	bool updateParticle(StandardParticle* particle) {
		if(start <= particle->lifetime.lifetimeElapsed() && particle->lifetime.lifetimeElapsed() <= end) {
			particle->render.size.normalize();
			particle->render.size *= radiusCurve->getValue(particle->lifetime.lifetimeElapsed());
		}
		return false;
	}

private:
	void init(Curve<float,float>* timeToRadiusCurve, float start, float end) {
		this->start = start;
		this->end = end;
		this->radiusCurve = timeToRadiusCurve;
	}
};




#endif /* RADIUSUPDATER_HPP_ */
