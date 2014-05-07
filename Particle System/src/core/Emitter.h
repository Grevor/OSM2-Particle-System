/*
 * Emitter.h
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef EMITTER_H_
#define EMITTER_H_

/**
 * Interface describing an emitter, capable of emitting information about a particle.
 *
 * Though the primary use is thought to be distributions of positions, the emitter can be used to emit other data as well.
 */
template<typename vectorType>
class Emitter {
	/**
	 * Emits a position (and possibly other data) for a particle-initialization.
	 * @return
	 * Data about a particle to be used in initialization.
	 */
	virtual vectorType emit() = 0;

	virtual ~Emitter() {}
};



#endif /* EMITTER_H_ */
