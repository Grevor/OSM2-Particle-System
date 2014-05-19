/*
 * Initializers.hpp
 *
 *  Created on: 16 maj 2014
 *      Author: Erik
 */

#ifndef INITIALIZERS_HPP_
#define INITIALIZERS_HPP_

#include "../Particle.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/random.hpp"

using namespace glm;


template<class init1, class init2>
class FusedInitializer {
	init1 i1;
	init2 i2;

	FusedInitializer(init1 i1, init2 i2) :
	i1(i1), i2(i2){}

	inline void init(Particle* p) {
		i1.init(p);
		i2.init(p);
	}
};


/**
 * Initializers.
 * These provide means to initialize particles.
 */
class RandomColorInitializer {
	unsigned char minColor[4];
	unsigned char deltaColor[4];

public:
	RandomColorInitializer(unsigned char minColor[4], unsigned char maxColor[4]) {

		for(int i= 0; i < 4; i++) {
			this->minColor[i] = minColor[i];
			this->deltaColor[i] = maxColor[i] - minColor[i];
		}
	}

	inline void init(Particle* p) {
		p->r = rand() % deltaColor[0] + minColor[0];
		p->g = rand() % deltaColor[1] + minColor[1];
		p->b = rand() % deltaColor[2] + minColor[2];
		p->a = rand() % deltaColor[3] + minColor[3];
	}
};

class RandomSpherePositionInitializer {
	vec3 middle;
	float delta, min;

public:
	RandomSpherePositionInitializer(vec3 middle, float minRadius, float maxRadius) {
		this->middle = middle;
		this->min = minRadius;
		this->delta = maxRadius - minRadius;
	}

	inline void init(Particle* p) {
		vec3 pos = glm::sphericalRand(delta);
		vec3 norm = pos;
		glm::normalize(norm);
		norm *= min;
		p->pos = pos + norm + middle;
	}
};

/**
 * In order to use this class, the position must be set.
 */
class VelocityFromOriginInitializer {
	float min, max;
public:
	VelocityFromOriginInitializer(float min, float max) {
		this->min = min;
		this->max = max;
	}

	inline void init(Particle* p) {
		vec3 norm = p->pos;
		glm::normalize(norm);
		p->speed = norm * linearRand(min,max);
	}
};

class VelocitySpreadInitializer {
	vec3 mainDir;
	float spread;
	float min,max;
public:
	VelocitySpreadInitializer(vec3 mainDir, float spread, float minSpeed, float maxSpeed) {
		this->mainDir = mainDir;
		this->mainDir = glm::normalize(this->mainDir);
		this->spread = spread;
		min = minSpeed;
		max = maxSpeed;
	}

	inline void init(Particle* p) {
		p->speed = mainDir * glm::linearRand(min,max) + spread * glm::sphericalRand<float>(1);
	}
};

class OffsetInitializer {
	vec3 offset;

public:
	OffsetInitializer(vec3 offset) {
		this->offset = offset;
	}

	inline void setOffset(vec3 newOffset) {
		offset = newOffset;
	}

	inline vec3 getOffset() {
		return offset;
	}

	inline void init(Particle* p) {
		p->pos +=offset;
	}
};

class ConstantLifetimeInitializer {
	float life;
public:
	ConstantLifetimeInitializer(float lifetime) {
		life = lifetime;
	}

	inline void setLifetime(float life) {
		this->life = life;
	}

	inline float getLifetime() { return life; }

	inline void init(Particle* p) {
		p->life = life;
		p->startLife = life;
	}
};

class RandomLifetimeInitializer {
	float min, delta;
public:
	RandomLifetimeInitializer(float min, float max) {
		this->min = min;
		this->delta = min - max;
	}

	inline void init(Particle* p) {
		p->life = min + delta * (float)rand() / RAND_MAX;// linearRand(0,1);
		p->startLife = p->life;
	}
};

class RandomSizeInitializer {
	float min, max;
public:
	RandomSizeInitializer(float min, float max) {
		this->min = min;
		this->max = max;
	}

	inline void init(Particle* p) {
		p->size = linearRand(min,max);
	}
};

class BasicInitializer {
public:
	inline void init(Particle* p) {
		memset(p,0,sizeof(Particle));
		p->weight = 1;
	}
};

class NullInitializer {
public:
	inline void init(Particle* p) {
		(void)p;
	}
};

#endif /* INITIALIZERS_HPP_ */