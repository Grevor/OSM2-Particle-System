/*
 * WaterParticle.h
 *
 *  Created on: 29 apr 2014
 *      Author: andreas 
 */

#include <glm/glm.hpp>

#ifndef WATERPARTICLESTRUCT_H_ 
#define WATERPARTICLESTRUCT_H_ 

typedef struct waterParticle_t {
    glm::vec3 color;
    glm::vec2 pos;
    glm::vec2 acceleration;
    glm::vec2 velocity;
    glm::vec2 prev_velocity;
	float density, pressure, mass;
    //flags
    int updatedPressure, updatedAcceleration, isStatic;
} *WaterParticle_t;


#endif /* WATERPARTICLESTRUCT_H_*/
