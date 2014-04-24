/*
 * ParticleUpdater.h
 *
 *  Created on: 17 apr 2014
 *      Author: David
 */

#ifndef PARTICLEUPDATER_H_
#define PARTICLEUPDATER_H_

/**
 * Interface describing an object which can update a particle of a specified type.
 */
template<typename Particle>
class ParticleUpdater {
public:
	/**
	 * Updates the given particle. The general contract is that this function may take any action.
	 * @param particle Pointer to the particle to update.
	 * @return
	 * True if the given particle is considered dead, else false.
	 */
	virtual bool updateParticle(Particle* particle) = 0;

	virtual ~ParticleUpdater() = 0;
};

#endif /* PARTICLEUPDATER_H_ */
