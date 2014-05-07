/*
 * ColorInitializer.h
 *
 *  Created on: 7 maj 2014
 *      Author: Elin
 */

#ifndef COLORINITIALIZER_H_
#define COLORINITIALIZER_H_

#include <Eigen/Eigen>
#include "StandardParticle.hpp"
#include "../ParticleInitializer.h"
#include <time.h>
#include <stdlib.h>

class ColorInitializer : public ParticleInitializer<StandardParticle>{
	const Vector3f* ColorVectorMin, ColorVectorMax, ColorVector;

public:
	Vector3f ColorInitializer(const Vector3f* ColorMin, const Vector3f* ColorMax){
		ColorVectorMin = ColorMin;
		ColorVectorMax = ColorMax;
		ColorVector = NULL;
		if((ColorMin[0]<= ColorMax[0]) && (ColorMin[1] <= ColorMax[1]) && (ColorMin[2] <= ColorMax[2])){
			for(int i = 0; i < 3; i++){
			float random = ((float) rand()) / ColorMax[i];
			float range = ColorMax[i]-ColorMin[i];
			ColorVector[i] = (random*range) + ColorMin[i];
			srand(time(NULL));
			}

		}
		return ColorVector;
	}
	ParticleInitializer(){}
}


#endif /* COLORINITIALIZER_H_ */
