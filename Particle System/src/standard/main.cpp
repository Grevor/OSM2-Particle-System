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
#include "EmitterInitializer.hpp"
#include "EmitterWithinSphere.hpp"
#include "ConstantForceUpdater.hpp"
#include "OffsetInitializer.hpp"
#include "PhysicsUpdater.hpp"
//#include "../ParticleEngine.h"
#include "OriginDistanceColorUpdater.hpp"
#include "FixedTimer.hpp"
#include "LifetimeUpdater.hpp"
#include "LifetimeInitializer.hpp"
#include "ReaperUpdater.hpp"
#include "LifetimeReaper.hpp"
#include "../core/ZeroInitializer.hpp"
#include "LifetimeColorUpdater.hpp"
#include "ExplosionVelocityInitializer.hpp"
//#include "RandomAlphaInitializer.hpp"
//#include "AlphaFadeUpdater.hpp"

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
	initializer->addInitializer(new ExplosionVelocityInitializer(&origin, 1.5));
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



