/*
 * SimpleParticleInitializer.h
 *
 *  Created on: 28 apr 2014
 */

#ifndef SIMPLEPARTICLEINITIALIZER_H_
#define SIMPLEPARTICLEINITIALIZER_H_

#include "ParticleInitializer.h"
#include "SimpleParticle.h"

class SimpleParticleInitializer : public ParticleInitializer<struct simpleParticle> {

public:
	SimpleParticleInitializer() {}

	void initParticle(struct simpleParticle* dst) override {
		dst->value = 0;
	}

	~SimpleParticleInitializer() override {

	}
};


#endif /* SIMPLEPARTICLEINITIALIZER_H_ */
