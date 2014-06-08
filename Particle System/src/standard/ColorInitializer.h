/*
 * ColorInitializer.h
 *
 *  Created on: 7 maj 2014
 */

#ifndef COLORINITIALIZER_H_
#define COLORINITIALIZER_H_

//#include <Eigen/Eigen>
#include "StandardParticle.hpp"
#include "../ParticleInitializer.h"
//#include <time.h>
//#include <stdlib.h>

class ColorInitializer : public ParticleInitializer<StandardParticle>{
	const Vector3f* colorVectorMin, colorVectorMax, colorVector;

public:
	void initParticle(StandardParticle* part) override{
		for(int i = 0; i < 3; i++){
			if((part->render.color[i] < colorVectorMin[i]) || (part->render.color[i] > colorVectorMax)){
				float random = ((float) rand()) / colorVectorMax[i];
				float range = colorVectorMax[i]-colorVectorMin[i];
				colorVector[i] = (random*range) + colorVectorMin[i];
			}else{
				colorVector[i] = part->render.color[i];
			}
		}
	}
	ColorInitializer(){
		srand(time(NULL));
		for(int i = 0; i < 3; i++){

			colorVector[i] = (((float) rand()) / 256);

			if(colorVector[i] > 20){
				colorVectorMin[i] = (colorVector[i] - 20);
			}else{
				colorVectorMin[i] = 0;
			}

			if(colorVector < 236){
				colorVectorMax[i] = (colorVector[1] + 20);
			}else{
				colorVectorMax[i] = 256;
			}
		}
	}

	ColorInitializer(StandardParticle* part, const Vector3f* min, const Vector3f* max){
		this->colorVector = part->render.color;
		this->colorVectorMin = min;
		this->colorVectorMax = max;
		srand(time(NULL));
	}

	Vector3f* colorInit(){
		if((colorVectorMin[0]<= colorVectorMax[0]) && (colorVectorMin[1] <= colorVectorMax[1]) && (colorVectorMin[2] <= colorVectorMax[2])){
			for(int i = 0; i < 3; i++){
			float random = ((float) rand()) / colorVectorMax[i];
			float range = colorVectorMax[i]-colorVectorMin[i];
			colorVector[i] = (random*range) + colorVectorMin[i];
			}
		}
		return colorVector;
	}
	void setLimit(Vector3f min, Vector3f max){
		colorVectorMax = max;
		colorVectorMin = min;
	}
};


#endif /* COLORINITIALIZER_H_ */
