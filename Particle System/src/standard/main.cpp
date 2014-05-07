/*
 * main.cpp
 *
 *  Created on: 2 maj 2014
 *      Author: Erik
 */

#include "StandardParticle.hpp"
#include <stdio.h>
#include <Eigen/Eigen>
//#include "StandardTimer.hpp"
#include "../../test/StringBitmap.hpp"
#include "../MultiInitializer.h"
#include "../MultiUpdater.h"
#include "../MultiReaper.h"
#include "../Curve.h"
#include "../ConstantCurve.h"
#include "../ParticleSystem.h"
#include "../NaiveParticlePool.h"

using namespace Particle;
using namespace Curves;

void printVector(Vector3f vec) {
	printf("(%f, %f, %f)", vec[0], vec[1], vec[2]);
}

int main(int argc, char **argv) {
	//StandardTimer timer = StandardTimer();

	MultiUpdater<StandardParticle>* updater;
	MultiInitializer<StandardParticle>* initializer;
	Curve<long,long>* spawnCurve = new ConstantCurve(1);

	ParticleSystem<StandardParticle>* system = new ParticleSystem<StandardParticle>(new NaiveParticlePool<StandardParticle>(4000), initializer, updater, spawnCurve, false);

	StandardParticle p;
	Vector3f force {3,4,3};
	p.vec.addForce(force);
	p.vec.updatePosAndVel(1);

	printf("Particle vectors:\n");
	printVector(p.vec.pos);
	printf("\n");
	printVector(p.vec.vel);
	printf("\n");
	printVector(p.vec.force);
	printf("\n");

	//printf("Timer shows: %f\n", timer.getTime());
	//usleep(3511111);
	//printf("Timer shows: %f\n", timer.getTime());

	StringBitmap bmp = StringBitmap(20,20);
	bmp.drawDot(3, 3);

	bmp.printBitmap();

	return 0;
}



