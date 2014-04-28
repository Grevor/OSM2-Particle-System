/*
 * demo.cpp
 *
 *  Created on: 27 apr 2014
 *      Author: Erik
 */


#include <stdio.h>
#include "ConstantCurve.h"
#include "SimpleParticleInitializer.h"
#include "SimpleParticleUpdater.h"
#include "SimpleParticle.h"
#include "ParticleSystem.h"
#include "NaiveParticlePool.h"

using namespace Curves;

#define ITER 10

int main(int argc, char **argv) {
	NaiveParticlePool<struct simpleParticle>* pool = new NaiveParticlePool<struct simpleParticle>(3);
	SimpleParticleInitializer* init = new SimpleParticleInitializer();
	SimpleParticleUpdater* update = new SimpleParticleUpdater();
	ConstantCurve c(1);
	ParticleSystem<struct simpleParticle>* system = new ParticleSystem<struct simpleParticle>(pool,init,update,&c,false);

	printf("Hello world!\n");

	printf("Managed to create curve on stack.\n");
	printf("Curve gives value %ld for input 45.\n", c.getValue(45));

	for(int i = 0; i < ITER; i++) {
		printf("Started step %d of %d for particle system.\n", i,ITER);
		system->step();
		system->update();
		ParticleIterator<struct simpleParticle>* living = system->getLivingParticles();
		struct simpleParticle* part;
		while(living->hasNext()) {
			part = living->next();
			printf("Particle value: %d\n", part->value);
			living->done(part);
		}
		delete living;
		printf("\n");
	}
	return 0;
}
