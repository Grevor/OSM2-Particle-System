#ifndef FLAGS_H_
#define FLAGS_H_

#include "IncCurve.h"

#define GRAVITY 9.82f
#define PI 3.141516f
#define E 2.71828f

#define CELL_SIZE 1.f
// pythagoras with a 0.1f "buffer" because floats
#define INIT_H 2.f*sqrt(CELL_SIZE * CELL_SIZE + CELL_SIZE * CELL_SIZE)+0.1f
#define INIT_VISCOSITY 0.3f
#define INIT_DAMPENING 0.15f
#define INIT_SPEED_OF_SOUND 0.07f
#define INIT_WATER_DENSITY 500.0f
#define INIT_MAX_DENSITY 1.37f

extern int DEATH;
extern float WATER_DENSITY;
extern float SPEED_OF_SOUND;
extern float MAX_DENSITY;
extern float TIME_STEP;
extern bool DEBUG;
extern float H;
extern float HH;
extern float VISCOSITY;
extern float DAMPENING;
extern IncCurve *INCCURVE;

#define NUM_WATER_PARTICLES 1000.f
#define SYSTEM_WIDTH 20.f
#define SYSTEM_HEIGHT 20.f
#define VOLUME SYSTEM_WIDTH*SYSTEM_HEIGHT

#define MAX_VELOCITY 100.f
#define MAX_ACCELERATION 100.f

#define POINT_SIZE 3.0f

#define RAND_DISTRIBUTION 1.5f

#define INIT_TIME_STEP 0.01f

#define WINDOW_X 800
#define WINDOW_Y 600

#define STATIC_PARTICLE_PRESSURE 0
#define STATIC_PARTICLE_DENSITY 0
#define STATIC_PARTICLE_X_COLOR 0
#define STATIC_PARTICLE_Y_COLOR 0
#define STATIC_PARTICLE_Z_COLOR 0

#endif
