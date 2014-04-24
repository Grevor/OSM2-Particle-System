/*
 * ParticleSystem.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

/**
 * Class representing a particle system which can be updated concurrently.
 */
template<typename Particle, typename UpdateData>
class ParticleSystem: Updateable<UpdateData> {
public:
	ParticleSystem();
	virtual ~ParticleSystem();
	/**
	 * Resets this ParticleSystem to its starting state. Useful for reuse of ParticleSystem:s
	 */
	void reset();
	/**
	 * Updates this particle system, making it progress to its next iteration if all
	 * updating from the previous frame has been completed.
	 * @param data
	 */
	void update(UpdateData data) override;
	/**
	 * Spawns, updates and reaps particles in this ParticleSystem for this iteration.
	 * Can be called by multiple threads to perform the work concurrently.
	 */
	void updateParticles();
	/**
	 * Check if this ParticleSystem is looping, indicating that it will keep spawning
	 * particles and never die, unless it's forced to do so.
	 * @return true if looping, else false
	 */
	bool isLooping();
	virtual bool isAlive() override;
};

#endif /* PARTICLESYSTEM_H_ */
