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
	virtual ParticleIterator<T> getAllocationIterator();
};

#endif /* PARTICLEPOOL_H_ */
