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
#include "ASCIIRenderer.hpp"
//#include "EmitterInitializer.hpp"
//#include "EmitterWithinSphere.hpp"
#include "ConstantForceUpdater.hpp"
#include "OffsetInitializer.hpp"
#include "PhysicsUpdater.hpp"

using namespace Particle;
using namespace Curves;

void printVector(Vector3f vec) {
	printf("(%f, %f, %f)", vec[0], vec[1], vec[2]);
}

int main(int argc, char **argv) {
	//StandardTimer timer = StandardTimer();

	Vector3f offset{10, 0 ,0};
	Vector3f pos {3,4,3};
	MultiUpdater<StandardParticle>* updater = new MultiUpdater<StandardParticle>(false);
	MultiInitializer<StandardParticle>* initializer = new MultiInitializer<StandardParticle>();
	Curve<long,long>* spawnCurve = new ConstantCurve(1);
	updater->addUpdater(new ConstantForceUpdater(pos));
	updater->addUpdater(new PhysicsUpdater());
	initializer->addInitializer(new OffsetInitializer(&offset));

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
	ParticleRenderer<StandardParticle>* renderer = new ASCIIRenderer(&bmp);

	for(int i = 0; i < 10; i++) {
		system->step();
		system->update();

		ParticleIterator<StandardParticle>* iter = system->getLivingParticles();

		while(iter->hasNext()) renderer->renderParticle(iter->next());
		delete iter;
		bmp.printBitmap();
		bmp.clear();
		printf("-----------------------------------------\n");
		sleep(1);
	}


	delete system;
	delete spawnCurve;
	delete updater;
	delete initializer;
	return 0;
}



