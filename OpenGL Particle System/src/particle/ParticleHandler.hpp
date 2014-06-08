/*
 * GLParticleInitializer.hpp
 *
 *  Created on: 16 maj 2014
 */

#ifndef PARTICLEHANDLER_HPP_
#define PARTICLEHANDLER_HPP_

#include "ParticleInitializer.h"
#include "ParticleUpdater.h"
#include "../Particle.hpp"
#include "Initializers.hpp"
#include "Updaters.hpp"

/**
 * Represents a certain class which contains up to 6 initializers and up to 6 updaters.
 *
 * All initializers must provide a void init(Particle* p) function.
 * All updaters must provide a bool update(Particle* p, double deltaTime) function.
 */
template<
	class init1, 					class init2 = NullInitializer, 	class init3 = NullInitializer,
	class init4 = NullInitializer, 	class init5 = NullInitializer, 	class init6 = NullInitializer,
	class update1 = NullUpdater, 	class update2 = NullUpdater, 	class update3 = NullUpdater,
	class update4 = NullUpdater,	class update5 = NullUpdater,	class update6 = NullUpdater>
class ParticleHandler : public ParticleUpdater<Particle>, public ParticleInitializer<Particle> {
	double lastTime, deltaTime;

public:
	init1 init_1;
	init2 init_2;
	init3 init_3;
	init4 init_4;
	init5 init_5;
	init6 init_6;

	update1 update_1;
	update2 update_2;
	update3 update_3;
	update4 update_4;
	update5 update_5;
	update6 update_6;

	ParticleHandler(double time, init1 i1,update1 u1) : init_1(i1), update_1(u1) {
		lastTime = time;
		deltaTime = 0;
	}

	ParticleHandler(double time, init1 i1, init2 i2, init3 i3, update1 u1) : init_1(i1), init_2(i2), init_3(i3), update_1(u1) {
		lastTime = time;
		deltaTime = 0;
	}

	ParticleHandler(double time, init1 i1, init2 i2, init3 i3, update1 u1, update2 u2) : init_1(i1), init_2(i2), init_3(i3), update_1(u1), update_2(u2) {
			lastTime = time;
			deltaTime = 0;
		}

	ParticleHandler(double time, init1 i1, init2 i2, init3 i3, init4 i4, update1 u1, update2 u2, update3 u3) :
		init_1(i1), init_2(i2), init_3(i3), init_4(i4), update_1(u1), update_2(u2), update_3(u3) {
			lastTime = time;
			deltaTime = 0;
		}

	void updateDelta(double time) {
		this->deltaTime = time - lastTime;
		lastTime = time;
	}

	void initParticle(Particle* p) override {
		init_1.init(p);
		init_2.init(p);
		init_3.init(p);
		init_4.init(p);
		init_5.init(p);
		init_6.init(p);
	}

	bool updateParticle(Particle* p) override {
		return  update_1.update(p, deltaTime) ||
				update_2.update(p, deltaTime) ||
				update_3.update(p, deltaTime) ||
				update_4.update(p, deltaTime) ||
				update_5.update(p, deltaTime) ||
				update_6.update(p, deltaTime);
	}
};

#endif /* PARTICLEHANDLER_HPP_ */
