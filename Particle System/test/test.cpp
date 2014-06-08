/*
 * demo.cpp
 *
 *  Created on: 27 apr 2014
 *      Author: Erik
 *
 *  Do note that this will only work under cygwin.
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../src/ConstantCurve.h"
#include "SimpleParticleInitializer.h"
#include "SimpleParticleUpdater.h"
#include "SimpleParticle.h"
#include "../src/ParticleSystem.h"
#include "../src/NaiveParticlePool.h"
#include "ParticleSystem_test.cpp"

using namespace Curves;

#define ITER 10
#define NUM_PARTICLES (1 << 22)
#define NUM_THREADS 1

bool isInArray(struct simpleParticle** arr, size_t length, struct simpleParticle* test) {
	for(size_t i = 0; i < length; i++)
		if(arr[i] == test) return true;
	return false;
}

struct foo {
	ParticleSystem<struct simpleParticle>* system;
};

void* workFunction(void* iter) {
	struct foo* foo = (struct foo*) iter;
	foo->system->update();
	delete foo;
	return NULL;
}

bool valFunc(struct simpleParticle* p, int i) {
	return true;
	//return p->value == i;
}

void startWorkerThread(ParticleSystem<struct simpleParticle>* system) {
	pthread_t thread;
	struct foo* fooStruct = new struct foo();
	fooStruct->system = system;
	pthread_create(&thread, NULL, &workFunction, (void*)fooStruct);
}

int main(int argc, char **argv) {
	NaiveParticlePool<struct simpleParticle>* pool = new NaiveParticlePool<struct simpleParticle>(NUM_PARTICLES);
	SimpleParticleInitializer* init = new SimpleParticleInitializer();
	SimpleParticleUpdater* update = new SimpleParticleUpdater(100);
	Curve<long,long>* c = new ConstantCurve(400);

	ParticleSystem<struct simpleParticle>* system = new ParticleSystem<struct simpleParticle>(pool,init,update,c,false);

	testParticleSystem(system,&valFunc,c,pool,NUM_PARTICLES,30);

	printf("Hello world!\n");

	printf("Managed to create curve on stack.\n");
	printf("Curve gives value %ld for input 45.\n", c->getValue(45));

	//struct simpleParticle** readParticles = (struct simpleParticle**) calloc(NUM_PARTICLES,sizeof(struct simpleParticle*));
	size_t currentPos = 0;

	for(int i = 0; i < ITER; i++) {
		currentPos = 0;
		printf("Started step %d of %d for particle system.\n", i + 1,ITER);
		system->step();

		for(int i = 0; i < NUM_THREADS; i++) startWorkerThread(system);
		ParticleIterator<struct simpleParticle>* living = system->getLivingParticles();

		struct simpleParticle* part;
		while(living->hasNext()) {
			part = living->next();
			//printf("Particle value: %d\n", part->value);
			//if(part->value != i + 1) printf("Read non-updated particle at round %d. %d\n", i + 1, part->value);
			//readParticles[currentPos] = part;
			currentPos++;
			living->done(part);
		}
		delete living;
		printf("%ld Particles this iteration.\n", currentPos);
	}
	//free(readParticles);
	return 0;
}
