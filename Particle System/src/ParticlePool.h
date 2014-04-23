/*
 * ParticlePool.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLEPOOL_H_
#define PARTICLEPOOL_H_

template<typename T>
class ParticlePool : ReadableParticlePool<T> {
public:
	virtual ParticleIterator<T> getAllocationIterator() = 0;

	virtual ~ParticlePool() = 0;
};

#endif /* PARTICLEPOOL_H_ */
