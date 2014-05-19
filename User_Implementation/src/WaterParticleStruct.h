/*
 * WaterParticle.h
 *
 *  Created on: 29 apr 2014
 *      Author: andreas 
 */

#ifndef WATERPARTICLESTRUCT_H_ 
#define WATERPARTICLESTRUCT_H_ 

typedef struct waterParticle_t {
	double force, density, pressure, color, mass;
    double *pos, *acceleration, *velocity, *prev_velocity;
    int updatedPressure, updatedAcceleration;
} *WaterParticle_t;


#endif /* WATERPARTICLESTRUCT_H_*/
