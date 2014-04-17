/*
 * ParticleIterator.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLEITERATOR_H_
#define PARTICLEITERATOR_H_

template<class T>
class ParticleIterator {
public:
	virtual T* next() = 0;

	virtual bool hasNext() = 0;

	virtual T* operator++() = 0;

	virtual ~ParticleIterator() = 0;
};

#endif /* PARTICLEITERATOR_H_ */
