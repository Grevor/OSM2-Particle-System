/*
 * ZeroInitializer.hpp
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef ZEROINITIALIZER_HPP_
#define ZEROINITIALIZER_HPP_

#include "../ParticleInitializer.h"
#include <string.h>

template<typename T>
class ZeroInitializer : public ParticleInitializer<T> {
public:
	void initParticle(T* particle) override {
		memset(particle, 0, sizeof(T));
	}
};




#endif /* ZEROINITIALIZER_HPP_ */
