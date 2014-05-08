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
	boost::mt19937 randomEngine;
	boost::random::uniform_int_distribution<int> range;
	boost::variate_generator<boost::mt19937, boost::random::uniform_int_distribution<int>> gener;
public:

	RandomAlphaInitializer(int min, int max)
	: randomEngine(time(NULL)), range(min,max), gener(randomEngine, range) {
		if(min > max) {
			int foo = min;
			min = max;
			max = foo;
			range(min,max);
			gener(randomEngine, range);
		}
	}

	void initParticle(StandardParticle* particle) override {
		particle->render.maxAlpha = (unsigned char) gener();

	}
};



#endif /* RANDOMALPHAINITIALIZER_HPP_ */
