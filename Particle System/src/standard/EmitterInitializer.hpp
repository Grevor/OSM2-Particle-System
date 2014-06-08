/*
 * EmitterInitializer.hpp
 *
 *  Created on: 7 maj 2014
 */

#ifndef EMITTERINITIALIZER_HPP_
#define EMITTERINITIALIZER_HPP_
#include "../utilities/Emitter.h"
#include "StandardParticle.hpp"
#include "../ParticleInitializer.h"

class EmitterInitializer : public ParticleInitializer<StandardParticle> {
	Emitter<Vector3f>* emitter;
public:
	EmitterInitializer(Emitter<Vector3f>* emitter) {
		this->emitter = emitter;
	}

	void initParticle(StandardParticle* particle) override {
		particle->vec.pos = emitter->emit();
	}
};


#endif /* EMITTERINITIALIZER_HPP_ */
