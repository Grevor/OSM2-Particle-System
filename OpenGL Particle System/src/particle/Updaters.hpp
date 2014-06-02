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
		OscilatingColorUpdater(.0,.5){

	}

	OscilatingColorUpdater(double minFreq, double maxFreq) :
		OscilatingColorUpdater(linearRand(minFreq, maxFreq),linearRand(minFreq, maxFreq), linearRand(minFreq, maxFreq)){

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
		getRandomColor(color, t);
	}

	void getRandomColor(unsigned char* color, double t) {
		color[0] = (unsigned char) (255.0 * (1 + sin(t * r)) * .5);
		color[1] = (unsigned char) (255.0 * (1 + sin(t * g)) * .5);
		color[2] = (unsigned char) (255.0 * (1 + sin(t * b)) * .5);
	}
};

class OscilatingPositionOffset {
public:
	vec3 getOscilatingYPos(vec3 axis, float oscFreq, float time, float oscRate, float middle) {
		float oscVal = calcOscVal(oscRate, oscFreq, time, middle);
		return vec3(0,oscVal,0);//glm::rotation(vec3(1,0,0),glm::normalize(axis)) * vec3(0,oscVal,0);
	}

	vec3 getOscilatingXPos(vec3 axis, float oscFreq, float time, float oscRate, float middle) {
		float oscVal = calcOscVal(oscRate, oscFreq, time, middle);
		return vec3(oscVal,0,0);//glm::rotation(vec3(1,0,0),glm::normalize(axis)) * vec3(oscVal,0,0);
	}

	vec3 getOscilatingZPos(vec3 axis, float oscFreq, float time, float oscRate, float middle) {
		float oscVal = calcOscVal(oscRate, oscFreq, time, middle);
		return vec3(0,0,oscVal);//glm::rotation(vec3(1,0,0),glm::normalize(axis)) * vec3(0,0,oscVal);
	}

private:
	float calcOscVal(float oscRate, float oscFreq, float time, float middle) {
		float oscVal = oscRate * sinf(time * oscFreq) + middle;
		return oscVal;
	}
};


template<typename T> T lerp(T s, float interpolation, T t) {
	return (T)((1- interpolation) * s + interpolation * t);
}

class AlphaFader {
	float startFade, endFade;
	unsigned char endAlpha;
	bool fitToLifetime;

public:
	AlphaFader(float start, float end, unsigned char target, bool fitToLifetime) {
		startFade = start;
		endFade = end;
		endAlpha = target;
		this->fitToLifetime = fitToLifetime;
	}

	unsigned char getAlpha(Particle* p) {
		if(fitToLifetime) {

			return 255;
		}
		else {
			char alpha = p->a;
			float interpolationVal = p->startLife - p->life;
			if(interpolationVal < startFade) return alpha;
			else if(interpolationVal > endFade) return endAlpha;
			else return lerp<unsigned char>(alpha, (interpolationVal - startFade) / (endFade - startFade), endAlpha);
		}
	}
};

template<typename T>
class Interpolator {
	float start,end;
	T target, startVal;

public:
	Interpolator() {}
	Interpolator(float start, float end, T target) :
		Interpolator(start,end,target, T(0)) { }

	Interpolator(float start, float end, T target, T startVal) {
		this->start = start;
		this->end = end;
		this->target = target;
		this->startVal = startVal;
	}


	bool isInInterval(float interpolationVal) {
		return start < interpolationVal && interpolationVal < end;
	}

	inline bool isBefore(float interpolationVal) {
		return start > interpolationVal;
	}

	inline bool isAfter(float interpolationVal) {
		return end < interpolationVal;
	}

	T getTarget() {
		return target;
	}

	T getInterpolatedValue(T current, float interpolationVal) {
			interpolationVal = (interpolationVal - start) / (end -start);
			return lerp<T>(current, interpolationVal, target);
		}

	T getInterpolatedValue(float interpolationVal) {
		return getInterpolatedValue(startVal,interpolationVal);
	}
};

class ColorGradient {
	Interpolator<unsigned char> inter[3];
public:
	ColorGradient() { }

	ColorGradient(float start, float end, unsigned char targetColor[3]) {
		for(int i = 0; i < 3; i++)
			inter[i] = Interpolator<unsigned char>(start,end,targetColor[i]);
	}

	void getColor(unsigned char* color, float interpolationValue) {
		for(int i = 0; i < 3; i++) {
			if(inter[i].isBefore(interpolationValue)) continue;
			else if(inter[i].isAfter(interpolationValue)) color[i] = inter[i].getTarget();
			else color[i] = inter[i].getInterpolatedValue(color[i],interpolationValue);
		}
	}
};

#endif /* UPDATERS_HPP_ */
