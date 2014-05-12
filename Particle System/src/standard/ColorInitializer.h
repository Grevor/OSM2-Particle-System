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
	const Vector3f* colorVectorMin, colorVectorMax, colorVector;

public:
	void initParticle(StandardParticle* part) override{
		this->colorVector = part->render.color;


	}

	ColorInitializer(StandardParticle* part, const Vector3f* min, const Vector3f* max){
		this->colorVector = part->render.color;
		this->colorVectorMin = min;
		this->colorVectorMax = max;
		srand(time(NULL));
	}

	Vector3f* ColorInit(){
		if((colorVectorMin[0]<= colorVectorMax[0]) && (colorVectorMin[1] <= colorVectorMax[1]) && (colorVectorMin[2] <= colorVectorMax[2])){
			for(int i = 0; i < 3; i++){
			float random = ((float) rand()) / colorVectorMax[i];
			float range = colorVectorMax[i]-colorVectorMin[i];
			colorVector[i] = (random*range) + colorVectorMin[i];
			}
		}
		return colorVector;
	}



};


#endif /* COLORINITIALIZER_H_ */
