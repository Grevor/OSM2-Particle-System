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
	double lastTime;
	double maxPerStep;
	double particlesPerSecond;

public:
	TimeCurve(double startTime, double maxPerStep, double particlesPerSecond) {
		this->lastTime = startTime;
		this->maxPerStep = maxPerStep;
		this->particlesPerSecond = particlesPerSecond;
	}

	void setIntensity(double particlesPerSecond, double maxParticlesPerUpdate) {
		this->particlesPerSecond = particlesPerSecond;
		this->maxPerStep = maxParticlesPerUpdate;
	}

	void addIntensity(double particlePerSecond, double maxParticlesPerUpdate) {
		setIntensity(this->particlesPerSecond + particlePerSecond, this->maxPerStep + maxParticlesPerUpdate);
	}

	long getValue(long unused) override {
		(void)unused;
		double time = glfwGetTime();
		double delta = time - lastTime;
		lastTime = time;

		long ans = delta * particlesPerSecond;
		if(ans > maxPerStep)
			ans = (long) maxPerStep;

		return ans;
	}
};



#endif /* TIMECURVE_HPP_ */