/*
 * ParticleUpdater.h
 *
 *  Created on: 17 apr 2014
 *      Author: David
 */

#ifndef PARTICLEUPDATER_H_
#define PARTICLEUPDATER_H_

template<class T>
class ParticleUpdater {
public:
	virtual bool updateParticle(T*) = 0;
};

#endif /* PARTICLEUPDATER_H_ */
