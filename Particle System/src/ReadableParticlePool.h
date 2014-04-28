/*
 * ReadableParticlePool.h
 *
 *  Created on: 17 apr 2014
 *      Author: Grevor
 */

#ifndef READABLEPARTICLEPOOL_H_
#define READABLEPARTICLEPOOL_H_

/**
 * Interface describing a particle pool which can be queried for living particles.
 */
template <typename T>
class ReadableParticlePool {
public:
	/**
	 * Gets an iterator to all currently living particles on a pool.
	 * This iterator guarantee that the particle is fully updated
	 * - to the discrepancy of one frame -
	 * and will not be altered until next or delete is called on this iterator.
	 * @return
	 * The iterator.
	 */
	virtual ParticleIterator<T>* getLivingIterator() = 0;

	virtual ~ReadableParticlePool() {}
};


#endif /* READABLEPARTICLEPOOL_H_ */
