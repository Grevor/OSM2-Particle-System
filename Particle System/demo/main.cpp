/*
 * main.cpp
 *
 *  Created on: 2 maj 2014
 */

#include "../src/standard/StandardParticle.hpp"
#include <stdio.h>
#include <Eigen/Eigen>
//#include "StandardTimer.hpp"
#include "../test/StringBitmap.hpp"
#include "../src/utilities/MultiInitializer.h"
#include "../src/utilities/MultiUpdater.h"
#include "../src/utilities/MultiReaper.h"
#include "../src/curves/Curve.h"
#include "../src/curves/ConstantCurve.hpp"
#include "../src/ParticleSystem.hpp"
#include "../src/NaiveParticlePool.hpp"
#include "../src/standard/ASCIIRenderer.hpp"
#include "../src/standard/EmitterInitializer.hpp"
#include "../src/standard/EmitterWithinSphere.hpp"
#include "../src/standard/ConstantForceUpdater.hpp"
#include "../src/standard/OffsetInitializer.hpp"
#include "../src/standard/PhysicsUpdater.hpp"
#include "../src/standard/OriginDistanceColorUpdater.hpp"
#include "../src/standard/FixedTimer.hpp"
#include "../src/standard/LifetimeUpdater.hpp"
#include "../src/standard/LifetimeInitializer.hpp"
#include "../src/standard/ReaperUpdater.hpp"
#include "../src/standard/LifetimeReaper.hpp"
#include "../src/utilities/ZeroInitializer.hpp"
#include "../src/standard/LifetimeColorUpdater.hpp"
#include "../src/standard/ExplosionVelocityInitializer.hpp"

using namespace Particle;
using namespace Curves;

void printVector(Vector3f vec) {
	printf("(%f, %f, %f)", vec[0], vec[1], vec[2]);
}

class FooColorInit : public ParticleInitializer<StandardParticle> {
	Vector3f vv;
public:
	FooColorInit(Vector3f& v) {
		vv = v;
	}

	void initParticle(StandardParticle* p) override {
		p->render.color = vv;
	}
};

int main(int argc, char **argv) {
	//StandardTimer timer = StandardTimer();

	Vector3f offset {20, 17 ,0};
	Vector3f origin {0, 1, 0};
	Vector3f pos {3, 4, 0};
	Vector3f gravity {0, .1, 0};
	Vector3f targetColor {'z', 0, 0};
	Vector3f initColor {'a', 0, 0};
	Vector3f zero {0, 0, 0};

	FixedTimer<float>* timer = new FixedTimer<float>(0);

	MultiUpdater<StandardParticle>* updater = new MultiUpdater<StandardParticle>(true);
	MultiInitializer<StandardParticle>* initializer = new MultiInitializer<StandardParticle>();
	Curve<long,long>* spawnCurve = new ConstantCurve(30);
	updater->addUpdater(new ReaperUpdater(new LifetimeReaper()));
	updater->addUpdater(new PhysicsUpdater());
	updater->addUpdater(new ConstantForceUpdater(gravity));
	//updater->addUpdater(new OriginDistanceColorUpdater(targetColor,0,30));
	updater->addUpdater(new LifetimeColorUpdater(targetColor, 0, 30));
	updater->addUpdater(new LifetimeUpdater(timer));
	//updater->addUpdater(new AlphaFader(false,true));

//	initializer->addInitializer(new RandomAlphaInitializer(1,122));
	initializer->addInitializer(new OffsetInitializer(&offset));
	initializer->addInitializer(new ExplosionVelocityInitializer(origin, 1.5));
	initializer->addInitializer(new EmitterInitializer(new EmitterWithinSphere(zero, 1)));
	initializer->addInitializer(new FooColorInit(initColor));
	initializer->addInitializer(new LifetimeUpdater(timer));
	initializer->addInitializer(new LifetimeInitializer(130));
	initializer->addInitializer(new ZeroInitializer<StandardParticle>());

	ParticleSystem<StandardParticle>* system = new ParticleSystem<StandardParticle>(
			new NaiveParticlePool<StandardParticle>(4000), initializer, updater, spawnCurve, false);
	//ParticleEngine* engine = new ParticleEngine(4);
	//engine->addParticleSystem(system);

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

	const int width = 40;

	StringBitmap bmp = StringBitmap(width + 1, 22);
	ParticleRenderer<StandardParticle>* renderer = new ASCIIRenderer(&bmp);

	bool right= true, moving = false;
	for(int i = 0; i < width * 4; i++) {
		timer->setTime(i);
		//engine->step();
		system->step();
		system->update();

		if(offset[0] >= width) right = false;
		else if(offset[0] <= 0) right = true;

		if(moving) {
			if(right) {
				offset[0]++;
				//origin[0]++;
			}
			else {
				offset[0]--;
				//origin[0]--;
			}
		}

		ParticleIterator<StandardParticle>* iter = system->getLivingParticles();

		while(iter->hasNext()) renderer->renderParticle(iter->next());
		delete iter;
		bmp.printBitmap();
		bmp.clear();
		printf("-----------------------------------------\n");
		usleep(1000000 / 10);
	}


	delete system;
	delete spawnCurve;
	delete updater;
	delete initializer;
	return 0;
}



