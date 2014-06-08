/*
 * RandomLifetimeInitializer.hpp
 *
 *  Created on: 5 maj 2014
 */

#ifndef RANDOMLIFETIMEINITIALIZER_HPP_
#define RANDOMLIFETIMEINITIALIZER_HPP_
#include "../ParticleInitializer.h"
#include "StandardParticle.hpp"
#include <boost/random.hpp>
#include <time.h>
#include <assert.h>

using namespace boost;

class RandomLifetimeInitializer : public ParticleInitializer<StandardParticle> {
	static mt19937 RandomEngine = mt19937(time(NULL));
	boost::uniform_real<float> range;
	boost::variate_generator<mt19937, uniform_real<float>> gen;
public:
	RandomLifetimeInitializer(float min, float max) {
		assert(min <= max);
		range(min,max);
		gen(RandomEngine, range);
	}

	void initParticle(StandardParticle* particle) override {
		particle->lifetime = gen();
	}
};


#endif /* RANDOMLIFETIMEINITIALIZER_HPP_ */
