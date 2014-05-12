/*
 * TimeCurve.hpp
 *
 *  Created on: 12 maj 2014
 *      Author: Erik
 */

#ifndef TIMECURVE_HPP_
#define TIMECURVE_HPP_

#include "Curve.h"
#include <GLFW/glfw3.h>

class TimeCurve : Curve<long,long> {
	double lastTime;
	double maxPerStep;
	double particlesPerSecond;

public:
	TimeCurve(double startTime, double maxPerStep, double particlesPerSecond) {
		this->lastTime = startTime;
		this->maxPerStep = maxPerStep;
		this->particlesPerSecond = particlesPerSecond;
	}

	long getValue(long unused) override {
		(void)unused;
		double time = glfwGetTime();
		double delta = time -lastTime;
		lastTime = time;

		long ans = delta * particlesPerSecond;
		if(ans > maxPerStep)
			ans = (long) maxPerStep;

		return ans;
	}
};



#endif /* TIMECURVE_HPP_ */
