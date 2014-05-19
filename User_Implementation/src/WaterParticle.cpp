#include "WaterParticle.h"
#include "WaterParticleStruct.h"
#include "../../Concurrent Collections/src/NeighbourTree.h"
#include <boost/foreach.hpp>
#include "Flags.h"
#include <algorithm>


WaterParticle::WaterParticle(int amountOfParticles, int volume, int density, double timeStep) {
    //mass = (density * total volume) / (total number of particles)
    //conservation of mass => we can store mass in class and not in each particle
    particleMass = (density * volume) / amountOfParticles;
    //particleMass = 500.0;
    tree = new NeighbourTree();
    current_time = 0.0;
    time_step = timeStep;
}

WaterParticle::~WaterParticle() {}

double WaterParticle::getTime(void) {
    return current_time;
}

void WaterParticle::RigidBodyCheck(WaterParticle_t particle) {
    double new_pos[3];
    new_pos[0] = particle->pos[0] + (particle->prev_velocity[0] * time_step);
    new_pos[1] = particle->pos[1] + (particle->prev_velocity[1] * time_step);
    new_pos[2] = particle->pos[2] + (particle->prev_velocity[2] * time_step);

    if (new_pos[1] < 0) {
        particle->prev_velocity[1] *= -0.6;
        //particle->prev_velocity[0] *= 10;
    }
    particle->pos[0] += particle->prev_velocity[0] * time_step;
    particle->pos[1] += particle->prev_velocity[1] * time_step;
    particle->pos[2] += particle->prev_velocity[2] * time_step;
}

void debugParticle(WaterParticle_t particle) {
    printf("Hello! I am particle with pointer: %p\n", particle);
    printf("Position: {%lf, %lf, %lf}\n", particle->pos[0], particle->pos[1], particle->pos[2]);
    printf("Acceleration: {%lf, %lf, %lf}\n", particle->acceleration[0], particle->acceleration[1], particle->acceleration[2]);
    printf("Pressure: %lf\n", particle->pressure);
    printf("Density: %lf\n", particle->density);
    printf("Flags: Pressure updated: %d, Acceleration updated: %d\n", particle->updatedPressure, particle->updatedAcceleration);
    printf("\n");
}

void WaterParticle::initParticle(WaterParticle_t particle) {
    double *pos = new double[3];
    double *temp_acc = new double[3] {0};
    double *temp_vel = new double[3] {0};
    double *temp_prev_vel = new double[3] {0};
    pos[0] = (double) (rand() % RAND_DISTRIBUTION);
    pos[1] = (double) (WINDOW_Y - (rand() % RAND_DISTRIBUTION));
    pos[2] = (double) (rand() % RAND_DISTRIBUTION);

    particle->pos = pos;
    particle->acceleration = temp_acc;
    particle->velocity= temp_vel;
    particle->prev_velocity= temp_prev_vel;
    particle->force = 0;
    particle->density = 1000;
    particle->pressure = 0;
    particle->color = 0;

    particle->updatedPressure = 0;
    particle->updatedAcceleration = 0;
    tree->initParticle(particle->pos, particle);
}

double weightingFunction_Poly6(void) {
    return 315.0/(64 * PI * pow(H, 9.0));
}

double weightingFunctionSpikyDiff(void) {
    return -45.0 / (PI*pow(H, 6.0));
}

double weightingFunctionLaplacian(void) {
    return 45.0 / (PI*pow(H, 6.0));
}



void WaterParticle::findAcceleration(WaterParticle_t main, WaterParticle_t neighbour, double distance) {

    double *p1 = main->pos;
    double *p2 = neighbour->pos;

    double normalized[3];
    
    //normalize it
    normalized[0] = p1[0]-p2[0];
    normalized[1] = p1[1]-p2[1];
    normalized[2] = p1[2]-p2[2];
    double squaredLength = normalized[0]*normalized[0] + normalized[1]*normalized[1] + normalized[2] * normalized[2];
    if (abs(squaredLength) < 1) {
        return;
    }
    double length = sqrt(squaredLength);
    normalized[0] = normalized[0]/length;
    normalized[1] = normalized[1]/length;
    normalized[2] = normalized[2]/length;

    // calculate the pressure gradients
    double a = -0.5 * particleMass * (main->pressure + neighbour->pressure);
    double b = weightingFunctionSpikyDiff() * (H-distance) * (H-distance);
    double c = 1.0 / (main->density * neighbour->density);

    double constant;
    constant = (a*b*c)/particleMass;

    normalized[0] *= constant;
    normalized[1] *= constant;
    normalized[2] *= constant;

    //add normalized to acceleration
    main->acceleration[0] += normalized[0];
    main->acceleration[1] += normalized[1];
    main->acceleration[2] += normalized[2];

    double d = VISCOSITY * weightingFunctionLaplacian() * (H-distance) * particleMass / neighbour->density;
    main->acceleration[0] += (neighbour->velocity[0] - main->velocity[0]) * (d/particleMass);
    main->acceleration[1] += (neighbour->velocity[1] - main->velocity[1]) * (d/particleMass);
    main->acceleration[2] += (neighbour->velocity[2] - main->velocity[2]) * (d/particleMass);

    if (isnan(main->acceleration[0]) || isnan(main->acceleration[1]) || isnan(main->acceleration[2])) {
        printf("MAIN: \n");
        debugParticle(main);                
        printf("NEIGHBOUR: \n");
        debugParticle(neighbour);
        printf("DISTANCE: \n %lf\n", distance);
        printf("a: %lf, b: %lf, c: %lf\n", a, b, c);
        printf("\n");
        sleep(5);
        printf("hello");
        printf("hello");
        printf("hello");
    }
}

bool WaterParticle::updateParticle(WaterParticle_t particle) {
    ReturnCarry neighbours = tree->getRadiusNeighbours(particle->pos, RADIUS_OF_NEIGHBOURS);    
    std::vector<value> resultIterator = neighbours->resultIterator;
    if (DEBUG) {
        printf("NEIGHBOUR SIZE: %ld\n", resultIterator.size());
    }
    int *distances = neighbours->distances;

    int i = 0;
    if (particle->updatedPressure != 1) {
        if (DEBUG) {
            debugParticle(particle);
        }
        if (resultIterator.size() > 0) {
            particle->density = 0.0;
        }
        for (unsigned long i = 0; i < resultIterator.size(); i++) {
            //WaterParticle_t neighbourParticle = (WaterParticle_t) v.second;
            double distance = distances[i];
            double c = pow(HH-distance*distance, 3);
            particle->density += particleMass * weightingFunction_Poly6() * c;
        }
        particle->pressure = SPEED_OF_SOUND * (particle->density - WATER_DENSITY);
        particle->density = (particle->density < 20) ? 20 : particle->density;
        particle->updatedPressure = 1;
        return false;
    }
    if (particle->updatedAcceleration != 1) {
        if (DEBUG) {
            debugParticle(particle);
        }
        //double acceleration[3] = {0};
        BOOST_FOREACH(const value &v, resultIterator) {
            WaterParticle_t neighbourParticle = (WaterParticle_t) v.second;
            double distance = distances[i];
            findAcceleration(particle, neighbourParticle, distance);

            // viscosity acceleration can be added if simulation isn't smooth enough
        }
        
        //apply gravity as well
        particle->acceleration[1] -= 10.0;
        particle->updatedAcceleration = 1;
        return false;
    }

    double *delta_v = particle->prev_velocity;
    particle->prev_velocity[0] += particle->acceleration[0]*time_step;
    particle->prev_velocity[1] += particle->acceleration[1]*time_step;
    particle->prev_velocity[2] += particle->acceleration[2]*time_step;

    // old pos is saved to update the neighbour tree, 
    // and to check if the line between them crosses a rigid body
    double old_pos[3]; 
    old_pos[0] = particle->pos[0];
    old_pos[1] = particle->pos[1];
    old_pos[2] = particle->pos[2];

    RigidBodyCheck(particle);
    /*
    particle->pos[0] += particle->prev_velocity[0] * time_step;
    particle->pos[1] += particle->prev_velocity[1] * time_step;
    particle->pos[2] += particle->prev_velocity[2] * time_step;
    */

    particle->velocity[0] = (delta_v[0] + particle->prev_velocity[0]) / 2;
    particle->velocity[1] = (delta_v[1] + particle->prev_velocity[1]) / 2;
    particle->velocity[2] = (delta_v[2] + particle->prev_velocity[2]) / 2;

    //time to update pressure and acceleration again
    particle->updatedPressure = 0;
    particle->updatedAcceleration = 0;
    current_time += time_step;

    //update neighbourtree
    tree->updateParticlePosition(old_pos, particle->pos, particle);
    if (DEBUG) {
        debugParticle(particle);
    }
    return false;
}
