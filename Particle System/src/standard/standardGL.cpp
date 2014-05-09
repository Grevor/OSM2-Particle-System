/*
 * standardGL.cpp
 *
 *  Created on: 8 maj 2014
 *      Author: Erik
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "StandardParticle.hpp"
#include <stdio.h>
#include <stdlib.h>
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
#include "SizeInitializer.hpp"
//#include "RandomAlphaInitializer.hpp"
//#include "AlphaFadeUpdater.hpp"
#include "StandardGLRenderer.hpp"
#define FPS 10
#define Width 1336
#define Height 768

#ifdef _WIN32
#define usleep(a) Sleep((a) / 1000)
#endif

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

ParticleSystem<StandardParticle>* createFountain(Timer<float>* timer) {
	//StandardTimer timer = StandardTimer();

	Vector3f offset {0, 0 ,0};
	Vector3f origin {0, -1, 0};
	Vector3f pos {3, 4, 0};
	Vector3f gravity {0, -.1, 0};
	Vector3f targetColor {0, 0, 1};
	Vector3f initColor {0, 0, .3};
	Vector3f zero {0, 0, 0};
	Vector3f size {1,1,1};

	//FixedTimer<float>* timer = new FixedTimer<float>(0);

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
	//initializer->addInitializer(new OffsetInitializer(&offset));
	initializer->addInitializer(new ExplosionVelocityInitializer(origin, 2.4));
	initializer->addInitializer(new EmitterInitializer(new EmitterWithinSphere(zero, 1)));
	initializer->addInitializer(new FooColorInit(initColor));
	initializer->addInitializer(new LifetimeUpdater(timer));
	initializer->addInitializer(new LifetimeInitializer(30));
	initializer->addInitializer(new SizeInitializer(size));
	initializer->addInitializer(new ZeroInitializer<StandardParticle>());

	return new ParticleSystem<StandardParticle>(
			new NaiveParticlePool<StandardParticle>(4000), initializer, updater, spawnCurve, false);

}

void error_callback(int error, const char* description)
{
	cerr << description << endl;
}

int main(int argc, char **argv) {
	if(glfwInit() == GL_FALSE) {
		glfwTerminate();
		cout << "Failed to launch glfw." << endl;
		exit(-1);
	}
	glfwSetErrorCallback(&error_callback);
	GLFWwindow* window = glfwCreateWindow(1366,768,"Demo fountain", NULL, NULL);
	glfwMakeContextCurrent(window);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FixedTimer<float>* timer = new FixedTimer<float>(0);
	ParticleSystem<StandardParticle>* system = createFountain(timer);
	StandardGLRenderer* renderer = new StandardGLRenderer();

	while(!glfwWindowShouldClose(window)) {
		system->step();
		system->update();
		timer->setTime(timer->getTime() + 1);
		ParticleIterator<StandardParticle>* iter = system->getLivingParticles();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(120,Width/Height, 1,1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(100,0,0,
				0,0,0,
				0,1,0);

		while(true) {
			StandardParticle* part;
			part = iter->next();
			if(part == NULL) break;
			renderer->renderParticle(part);
		}
		delete iter;

		glfwSwapBuffers(window);
		glfwPollEvents();
		usleep(1000000 / FPS);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}



