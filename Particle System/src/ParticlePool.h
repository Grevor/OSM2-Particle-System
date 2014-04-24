/*
 * ParticlePool.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLEPOOL_H_
#define PARTICLEPOOL_H_

/**
 * Interface describing a particle pool which can be
 */
template<typename Particle>
class ParticlePool : public ReadableParticlePool<Particle> {
public:
	/**
	 * Gets an iterator which can be queried for particle allocations.
	 * @return
	 * The iterator.
	 */
	virtual ParticleIterator<Particle> getAllocationIterator() = 0;

	/**
	 * Returns the specified particle to the pool.
	 * @param p The particle to return to the pool.
	 * After this action is done, any pointer to the particle may not be used.
	 */
	virtual void returnParticle(Particle* p);

	/**
	 * Resets the particle-pool, releasing all particles contained within it.
	 * After this function is called, all pointers to any particle in the pool
	 * must be considered invalid.
	 */
	virtual void reset() = 0;

	virtual ~ParticlePool() = 0;
};

#endif /* PARTICLEPOOL_H_ */
