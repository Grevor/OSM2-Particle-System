/*
 * OscillationCurve.h
 *
 *  Created on: 1 maj 2014
 */

#ifndef OSCILLATIONCURVE_H_
#define OSCILLATIONCURVE_H_

#include <boost/random.hpp>
#include <ctime>
#include "Curve.h"

namespace Curves {

class OscillationCurve : public Curve<float, float> {
	//This random generator is shared for all OscillationCurves.
	static boost::mt19937 RandomGenerator(time());

	boost::uniform_real<> distribution;
	boost::variate_generator<boost::mt19937*, boost::uniform_real<>> random;
	float rateMiddle, rateDiff;

	inline float getFreq() {
		return distribution();
	}

	inline float getRateMiddle() {
		return rateMiddle;
	}

	inline float getRandomIntervalSize() {
		return rateDiff;
	}

public:
	OscillationCurve(float rateMin, float rateMax, float freqMin, float freqMax) {
		distribution(freqMin, freqMax);
		random(&RandomGenerator, distribution);
		rateMiddle = rateMin + rateMax / 2;
		rateDiff = rateMax - rateMin;
	}

	float getValue(float value) override {
		return sinf(value * getFreq()) * getRandomIntervalSize() + getRateMiddle();
	}
};

}



#endif /* OSCILLATIONCURVE_H_ */
