/*
 * ParticleSystem.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

template<class T>
class ParticleSystem : Updateable<T> {
public:
	ParticleSystem();
	virtual ~ParticleSystem();
};

#endif /* PARTICLESYSTEM_H_ */
