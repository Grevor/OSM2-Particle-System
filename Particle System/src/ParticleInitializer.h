/*
 * ParticleInitializer.h
 *
 *  Created on: 17 apr 2014
 *      Author: David
 */

#ifndef PARTICLEINITIALIZER_H_
#define PARTICLEINITIALIZER_H_

template<class T>
class ParticleInitializer {
public:
	virtual void initParticle(T*) = 0;

	virtual ~ParticleInitializer() = 0;
};

#endif /* PARTICLEINITIALIZER_H_ */
