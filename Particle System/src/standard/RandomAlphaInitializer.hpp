/*
 * RandomAlphaInitializer.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef RANDOMALPHAINITIALIZER_HPP_
#define RANDOMALPHAINITIALIZER_HPP_
#include "StandardParticle.hpp"
#include "../ParticleInitializer.h"
#include <boost/random.hpp>
#include <time.h>

using namespace boost;

class RandomAlphaInitializer : public ParticleInitializer<StandardParticle> {
	static mt19937 RandomEngine = mt19937(time(NULL));
	uniform_int<unsigned char> range;
	variate_generator<mt19937, uniform_int<unsigned char>> gen;
public:
	//Unused as of yet.
	static const int minAlpha = 1, maxAlpha = 2, alpha = 4;

	RandomAlphaInitializer(unsigned char min, unsigned char max) {
		if(min > max) {
			unsigned char foo = min;
			min = max;
			max = min;
		}
		range(min,max);
		gen(RandomEngine, range);
	}

	void initializeParticle(StandardParticle* particle) override {
		particle->render.maxAlpha = gen();
	}
};



#endif /* RANDOMALPHAINITIALIZER_HPP_ */
