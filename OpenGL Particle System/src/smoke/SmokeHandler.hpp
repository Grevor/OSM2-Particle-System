/*
 * SmokeHandler.hpp
 *
 *  Created on: 28 maj 2014
 *      Author: Erik
 */

#ifndef SMOKEHANDLER_HPP_
#define SMOKEHANDLER_HPP_

#include "SmokeParticle.hpp"
#include <ParticleInitializer.h>
#include <ParticleUpdater.h>
#include "../particle/Initializers.hpp"

class SmokeHandler : public ParticleUpdater<SmokeParticle>, public ParticleInitializer<SmokeParticle> {
	Interpolator<unsigned char> fadeIn, fadeOut;
	Interpolator<float> expand, retract;
	VelocitySpreadInitializer speed;
	ColorGradient startGrad;
	RandomSpherePositionInitializer pos;
	RandomLifetimeInitializer life;
	float delta;
public:
	vec3 outsideForce;
	SmokeHandler(vec3 pos, unsigned char maxAlpha, float minVel, float maxVel, float spread) :
	speed(vec3(0,1,0), spread, minVel, maxVel),
	pos(pos,.1,.4), life(2.5,3) {
		delta = 0;
		fadeIn = Interpolator<unsigned char>(0, .4, maxAlpha);
		fadeOut = Interpolator<unsigned char>(.5, 1, 0, maxAlpha);
		expand = Interpolator<float>(0, .7, 1.2);
		retract = Interpolator<float>(.9, 1, 0, 5);
		//unsigned char colorMin[3] {0,0,0};
		unsigned char colorMax[3] {180,180,180};
		startGrad = ColorGradient(0,1,colorMax);
	}

	void setDelta(float d) { delta = d; }

	void initParticle(SmokeParticle* p) override {
		p->a = 0;
		p->size = linearRand<float>(.3,.6);
		p->r = 0;
		p->g = 0;
		p->b = 0;
		startGrad.getColor(&p->r,linearRand<float>(0,1));
		p->pos = pos.getRandomSpherePosition();
		p->life = life.getRandomLifetime();
		p->startLife = p->life;
		p->speed = speed.getRandVelocitySpread();
		p->a = 0;
	}

	bool updateParticle(SmokeParticle* p) override {
		p->life -= delta;
		if(p->life <= 0) return true;

		float inter = (p->startLife - p->life) / p->startLife;
		p->a = fadeIn.getInterpolatedValue(inter);
		p->a = fadeOut.getInterpolatedValue(p->a, inter);
		p->size = expand.getInterpolatedValue(0,inter);
		//p->size = retract.getInterpolatedValue(p->size, inter);
		p->speed += (vec3(0,1.3,0) + outsideForce) * delta;
		p->pos += p->speed * delta;

		return false;
	}
};



#endif /* SMOKEHANDLER_HPP_ */
