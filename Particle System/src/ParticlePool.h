/*
 * ParticlePool.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLEPOOL_H_
#define PARTICLEPOOL_H_

template<class T>
class ParticlePool {
public:
	virtual ParticleIterator<T> getAllocationIterator() = 0;
};

#endif /* PARTICLEPOOL_H_ */
