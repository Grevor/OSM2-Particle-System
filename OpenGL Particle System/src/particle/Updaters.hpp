/*
 * Updaters.hpp
 *
 *  Created on: 16 maj 2014
 *      Author: Erik
 */

#ifndef UPDATERS_HPP_
#define UPDATERS_HPP_

#include "../Particle.hpp"
#include "glm/glm.hpp"

template<class updater1, class updater2>
class MultiUpdater {
	updater1 u_1;
	updater2 u_2;
public:
	MultiUpdater(updater1 u1, updater2 u2) {
		u_1 = u1;
		u_2 = u2;
	}

	inline bool update(Particle* p, double delta) {
		return u_1.update(p,delta) || u_2.update(p,delta);
	}
};

class NullUpdater {
public:
	inline bool update(Particle* p, double d) {
		(void)d;
		(void)p;
		return false;
	}
};

class ConstantForceUpdater {
	vec3 constantForce;
	bool weightDependant;

public:
	ConstantForceUpdater(vec3 force, bool weightDependant) {
		constantForce = force;
		this->weightDependant = weightDependant;
	}

	bool update(Particle* p, double deltaTime) {
		vec3 deltaSpeed = constantForce * (float)deltaTime;
		if(weightDependant) deltaSpeed / p->weight;
		p->speed += deltaSpeed * .5f;
		return false;
	}
};

class LifetimeUpdater {
public:
	bool update(Particle* p, double deltaTime) {
		p->life -=deltaTime;
		if(p->life < 0) return true;

		p->animationFrame =  1- (p->life / p->startLife);
		return false;
	}
};

class PhysicsUpdater {
public:
	bool update(Particle* p, double deltaTime) {
		p->pos += p->speed * (float)deltaTime;
		return false;
	}
};

class LifetimeReaper {
public:
	bool update(Particle* p, double delta) {
		(void)delta;
		return p->life < 0;
	}
};

class OscilatingColorUpdater {
	double t;
	double r,g,b;
public:

	OscilatingColorUpdater() :
	OscilatingColorUpdater(linearRand(0.f,5.f),linearRand(0.f,5.f), linearRand(0.f,5.f)){

	}

	OscilatingColorUpdater(double oscR, double oscG, double oscB) {
		t = 0;
		r = oscR;
		g = oscG;
		b = oscB;
	}

	void addTime(double delta) {
		this->t += delta;
	}

	void getRandomColor(unsigned char* color) {
		color[0] = (unsigned char) (255.0 * (1 + sin(t * r)) * .5);
		color[1] = (unsigned char) (255.0 * (1 + sin(t * g)) * .5);
		color[2] = (unsigned char) (255.0 * (1 + sin(t * b)) * .5);
	}
};


#endif /* UPDATERS_HPP_ */
