/*
 * ParticleEngine_test.cpp
 *
 *  Created on: 2 maj 2014
 *      Author: David
 */

#include <ParticleEngine.h>
#define BOOST_TEST_MODULE ParticleEngine_test
//#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#define ITERATIONS 100

struct IterUpdateableTest : IterationUpdateable {
	std::atomic<int> counter = 0;
public:
	void step() override {
		while (!isStepComplete) {}
		counter = 100000;
	}

	bool isStepComplete() override {
		return counter = 0;
	}

	void update() override {
		while (counter > 0) {
			--counter;
		}
	}

	bool isAlive() {
		return true;
	}
};

BOOST_AUTO_TEST_CASE( test_case1 )
{
	ParticleEngine* pEng = new ParticleEngine();
	int nPSys = 10;
	IterUpdateableTest* pSystems[nPSys];
	for (int i = 0; i < nPSys; ++i) {
		pEng->addParticleSystem(pSystems[i] = new IterUpdateableTest());
	}
	for (int iteration = 0; iteration < ITERATIONS; ++iteration) {
		pEng->step();
		pEng->waitStepComplete();
		for (int i = 0; i < nPSys; ++i) {
			BOOST_CHECK(pSystems[i]->isStepComplete());
		}
	}
	for (int i = 0; i < nPSys; i += 2) {
		pEng->removeParticleSystem(pSystems[i]);
		pSystems[i]->counter = 1337;
	}
	pEng->step();
	pEng->waitStepComplete();
	for (int i = 0; i < nPSys; i += 2) {
		BOOST_CHECK(pSystems[i]->isStepComplete() == false);
	}
	for (int i = 1; i < nPSys; i += 2) {
		BOOST_CHECK(pSystems[i]->isStepComplete());
	}
	for (int i = 0; i < nPSys; ++i) {
		delete pSystems[i];
	}
	delete pEng;
}
