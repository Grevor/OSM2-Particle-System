/*
 * DustSphere.hpp
 *
 *  Created on: 19 maj 2014
 *      Author: Erik
 */

#ifndef DUSTSPHERE_HPP_
#define DUSTSPHERE_HPP_

#include "Curve.h"
#include "ParticleInitializer.h"
#include "ParticleUpdater.h"
#include "../particle/Initializers.hpp"
#include "../Particle.hpp"

namespace DustSphere {

class SphereInitializer : public ParticleInitializer<Particle>{
	OnSphereInitializer sphere;
	RandomColorInitializer color;
	float size;
	unsigned char c[4];
public:
	SphereInitializer(vec3 pos, float rad, unsigned char color[4], float size) : sphere(pos,rad), color(color,color) {
		this->size = size;
	}

	void initParticle(Particle* p) override {
		p->pos = sphere.getPosition();
		color.getRandomColor(&p->r);
		p->size = size;
		p->speed = vec3(0,0,0);
		p->weight = 1;

	}
};

class AttractorUpdater : public ParticleUpdater<Particle> {
	float power;
	vec3 attractor, middle;
	float delta;
	float radius;
	OscilatingColorUpdater colorUpdater;
public:
	AttractorUpdater(vec3 attractorPos, vec3 middle, float rad) {
		power = 0;
		this->attractor = attractorPos;
		this->delta = 0;
		radius = rad;
		this->middle = middle;
	}

	void setAttractorPos(vec3 newPos) { attractor = newPos; }

	vec3 getAttractorPos() {
		 return attractor;
	}

	void setAttractorPower(float power) {
		this->power = power;
	}

	void setDeltaTime(float delta) { this->delta = delta;
		colorUpdater.addTime(delta);
	}

	bool updateParticle(Particle* p) override {
		//Update. Yes, its strange as hell.
		vec3 deltaPos = attractor - p->pos;
		if(power != 0) p->speed += normalize(deltaPos) * power * delta;// * length(deltaPos) * .5f;
		//else p->speed += vec3(0,-delta * .05,0);
		p->pos += p->speed * delta;

		//Check max distance
		vec3 deltaMiddle = p->pos - middle;

		if(length(deltaMiddle) > radius) {
			p->pos = normalize(deltaMiddle) * radius + middle;
			//p->speed = vec3(0,0,0);
		}

		colorUpdater.getRandomColor(&p->r);
		return false;
	}
};



class AllAtOnceSpawnCurve : public Curves::Curve<long,long> {
	long p;
public:
	AllAtOnceSpawnCurve(long particles) {
		p = particles;
	}

	long getValue(long currentIter) override {
		if(currentIter <= 1) return p;
		else return 0;
	}
};

}



#endif /* DUSTSPHERE_HPP_ */
