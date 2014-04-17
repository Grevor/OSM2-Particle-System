/*
 * ReadableParticlePool.h
 *
 *  Created on: 17 apr 2014
 *      Author: Grevor
 */

#ifndef READABLEPARTICLEPOOL_H_
#define READABLEPARTICLEPOOL_H_

template <class T>
class ReadableParticlePool {
public:
	virtual ParticleIterator<T> getLivingIterator() = 0;

	virtual ~ReadableParticlePool() = 0;
};


#endif /* READABLEPARTICLEPOOL_H_ */
