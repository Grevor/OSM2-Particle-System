/*
 * ParticleInitializer.h
 *
 *  Created on: 17 apr 2014
 *      Author: David
 */

#ifndef PARTICLEINITIALIZER_H_
#define PARTICLEINITIALIZER_H_

/**
 * Interface describing an object which can initialize particles of a specified type.
 */
template<typename Particle>
class ParticleInitializer {
public:
	/**
	 * Initialises a particle at the given memory location
	 * @param dst is a pointer to the preallocated memory where the
	 * new particle will be stored.
	 */
	virtual void initParticle(Particle* dst) = 0;

	virtual ~ParticleInitializer() = 0;
};

#endif /* PARTICLEINITIALIZER_H_ */
