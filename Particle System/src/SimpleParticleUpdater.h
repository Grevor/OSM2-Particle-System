/*
 * SimpleParticleUpdater.h
 *
 *  Created on: 28 apr 2014
 *      Author: Erik
 */

#ifndef SIMPLEPARTICLEUPDATER_H_
#define SIMPLEPARTICLEUPDATER_H_

#include "SimpleParticle.h"
#include "ParticleUpdater.h"

#define PARTICLE_MAX_VAL 5

/**
 * Increments value by 1.
 */
class SimpleParticleUpdater : public ParticleUpdater<struct simpleParticle> {
	int maxVal;
public:
	SimpleParticleUpdater(int max) { maxVal = max; }

	bool updateParticle(struct simpleParticle* particle) override {
		if(particle->value >= maxVal) return true;

		particle->value++;
		return false;
	}

	~SimpleParticleUpdater() override {

	}
};


#endif /* SIMPLEPARTICLEUPDATER_H_ */
