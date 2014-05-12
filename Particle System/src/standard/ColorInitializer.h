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
	void initParticle(StandardParticle* part) override{
		this->ColorVector = part->render.color;


	}

	ColorInitializer(StandardParticle* part, const Vector3f* min, const Vector3f* max){
		this->ColorVector = part->render.color;
		this->ColorVectorMin = min;
		this->ColorVectorMax = max;
		srand(time(NULL));
	}

	Vector3f* ColorInit(){
		if((ColorVectorMin[0]<= ColorVectorMax[0]) && (ColorVectorMin[1] <= ColorVectorMax[1]) && (ColorVectorMin[2] <= ColorVectorMax[2])){
			for(int i = 0; i < 3; i++){
			float random = ((float) rand()) / ColorVectorMax[i];
			float range = ColorVectorMax[i]-ColorVectorMin[i];
			ColorVector[i] = (random*range) + ColorVectorMin[i];
			}
		}
		return ColorVector;
	}



};


#endif /* COLORINITIALIZER_H_ */
