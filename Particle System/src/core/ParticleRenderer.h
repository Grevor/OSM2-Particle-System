/*
 * ParticleRenderer.h
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef PARTICLERENDERER_H_
#define PARTICLERENDERER_H_

/**
 * Interface describing an object which can render particles.
 */
template<typename T>
class ParticleRenderer {
public:
	/**
	 * Renders the specified particle.
	 * @param particle The particle to render.
	 */
	virtual void renderParticle(T* particle) = 0;

	virtual ~ParticleRenderer() {}
};



#endif /* PARTICLERENDERER_H_ */
