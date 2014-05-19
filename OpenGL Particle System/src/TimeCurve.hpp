/*
 * TimeCurve.hpp
 *
 *  Created on: 12 maj 2014
 *      Author: Erik
 */

#ifndef TIMECURVE_HPP_
#define TIMECURVE_HPP_

#include <Curve.h>
#include <GLFW/glfw3.h>

class TimeCurve : public Curve<long,long> {
	double maxPerStep;
	double particlesPerSecond;
	long ans;

public:
	TimeCurve(double maxPerStep, double particlesPerSecond) {
		this->maxPerStep = maxPerStep;
		this->particlesPerSecond = particlesPerSecond;
		this->ans = 0;
	}

	void setIntensity(double particlesPerSecond, double maxParticlesPerUpdate) {
		this->particlesPerSecond = particlesPerSecond;
		this->maxPerStep = maxParticlesPerUpdate;
	}

	void addIntensity(double particlePerSecond, double maxParticlesPerUpdate) {
		setIntensity(this->particlesPerSecond + particlePerSecond, this->maxPerStep + maxParticlesPerUpdate);
	}

	void update(double delta) {
		ans = delta * particlesPerSecond;
		if(ans > maxPerStep)
			ans = (long) maxPerStep;
	}

	long getValue(long unused) override {
		(void)unused;
		return ans;
	}
};



#endif /* TIMECURVE_HPP_ */
