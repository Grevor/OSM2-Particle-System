#include "WaterParticle.h"
#include "WaterParticleStruct.h"
#include <boost/foreach.hpp>
#include "Flags.h"
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

float H;
float HH;
float VISCOSITY;
float DAMPENING;
int DEATH = 0;

void WaterParticle::initLines(void) {
    lines.push_back(line(glm::vec2(0.f, 6.f), glm::vec2(1.f, -0.5f)));
    lines.push_back(line(glm::vec2(0.f, 2.f), glm::vec2(1.f, 0.f)));
    lines.push_back(line(glm::vec2(2.f, 2.f), glm::vec2(0.f, 1.f)));
    lines.push_back(line(glm::vec2(SYSTEM_WIDTH - 2.f, 2.f), glm::vec2(0.f, 1.f)));
}

WaterParticle::WaterParticle(int amountOfParticles, float volume, float density, float timeStep, Cellist *c) {
    //mass = (density * total volume) / (total number of particles)
    //conservation of mass => we can store mass in class and not in each particle
    //particleMass = (density * volume) / amountOfParticles;
    //particleMass = 1.0f;
    particleMass = 3.25e-14f;
    this->cellList = c;
    current_time = 0.0; 
    time_step = timeStep;
    initLines();
}

std::vector<struct line> WaterParticle::getLines(void) {
    return lines;
}

WaterParticle::~WaterParticle() {}

float WaterParticle::getTime(void) {
    return current_time;
}

void debugParticle(WaterParticle_t particle) {
    printf("Hello! I am particle with pointer: %p\n", particle);
    printf("Position: {%lf, %lf}\n", particle->pos[0], particle->pos[1]);
    printf("Acceleration: {%lf, %lf}\n", particle->acceleration[0], particle->acceleration[1]);
    printf("Pressure: %lf\n", particle->pressure);
    printf("Density: %lf\n", particle->density);
    printf("Flags: Pressure updated: %d, Acceleration updated: %d\n", particle->updatedPressure, particle->updatedAcceleration);
    printf("\n");
}

void WaterParticle::initParticle(WaterParticle_t particle) {
    particle->color = glm::vec3(0,1,1);
    float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    particle->pos = glm::vec2(3 + x * RAND_DISTRIBUTION, 15 + y*RAND_DISTRIBUTION);
    particle->acceleration = glm::vec2(0.1f);
    particle->velocity = glm::vec2(0.1f);
    particle->prev_velocity = glm::vec2(0.1f);

    particle->density = 1.f;
    particle->pressure = 1.f;

    particle->updatedPressure = 0;
    particle->updatedAcceleration = 0;
    particle->isStatic = 0;
    cellList->insert(&particle->pos[0], particle);
}

float weightingFunction_Poly6(void) {
    return 315.0/(64 * PI * powf(H, 9.0));
}

float weightingFunctionSpikyDiff(void) {
    return -45.0 / (PI*powf(H, 6.0));
}

float weightingFunctionLaplacian(void) {
    return 45.0 / (PI*powf(H, 6.0));
}

float weightingFunction_Poly6_Del(void) {
    return -945.0f / (32.0f * PI * powf(H, 9.0f));
}

bool WaterParticle::findAcceleration(WaterParticle_t main, WaterParticle_t neighbour, float distance) {
    float a = -1.0f * (main->pressure/(main->density*main->density) + neighbour->pressure/(neighbour->density*neighbour->density));
    //kernel argument calculation
    float b = (HH-distance*distance)*(HH-distance*distance);
    //kernel calculation
    float c = weightingFunction_Poly6_Del() * distance;
    float d = VISCOSITY * weightingFunctionLaplacian() * (H-distance) * particleMass / neighbour->density;

    /*
    float a = -0.5f*particleMass*(main->pressure+neighbour->pressure);
    float b = weightingFunctionSpikyDiff()*(H-distance)*(H-distance);
    float c = 1.0f/(main->density*neighbour->density); 
    */

    if (isnan(a) || isnan(b) || isnan(c) || isnan(d)) {
        printf("ruh roh\n");
        return true;
    }

    main->acceleration += glm::normalize(main->pos-neighbour->pos)*(a*b*c);
    main->acceleration += (neighbour->velocity - main->velocity) * (d/particleMass);
    if (main->acceleration.length() > MAX_ACCELERATION) {
        main->acceleration = glm::normalize(main->acceleration) * MAX_ACCELERATION;
    }
    return false;
}

void WaterParticle::RigidBodyCheck(WaterParticle_t particle) {
    glm::vec2 old_pos = particle->pos;
    //glm::vec2 new_pos = particle->pos + particle->prev_velocity * time_step;
    BOOST_FOREACH(const line &v, getLines()) {
        /* equations */
        float dotProd = glm::dot(particle->prev_velocity, v.normal);
        float l2 = glm::dot(v.pos - old_pos, v.normal) / dotProd;
        if (l2 >= 0.0f && l2 <= 0.15f) {
            particle->prev_velocity[1] *= DAMPENING;
            particle->prev_velocity[1] -= GRAVITY * TIME_STEP;
            particle->prev_velocity = glm::reflect(particle->prev_velocity, v.normal);
        }
    }
}

bool WaterParticle::updateParticle(WaterParticle_t particle) {
    //cellList locks our stuff. BE SURE TO UNLOCK
    std::list<LockedList> *neighbours;
    if (particle->updatedPressure != 1 || particle->updatedAcceleration != 1) {
        neighbours = cellList->get(&particle->pos[0]);
    }
    if (particle->updatedPressure != 1) {
        particle->density = 1.0f;
        for (std::list<LockedList>::iterator Lists=neighbours->begin(); Lists!=neighbours->end(); ++Lists) {
            for (std::list<void*>::iterator it=(*Lists)->Particles->begin(); it!=(*Lists)->Particles->end(); ++it) {
                WaterParticle_t neighbourParticle = (WaterParticle_t) (*it);
                float distance = sqrt(powf(neighbourParticle->pos[0] - particle->pos[0], 2) + powf(neighbourParticle->pos[1] - particle->pos[1], 2));
                float c = powf(HH-distance*distance, 3.0f);
                particle->density += particleMass * weightingFunction_Poly6() * c;
                //We're done with the bucket, unlock it.
            }
            (*Lists)->Lock.unlock();
        }
        particle->density = std::max(particle->density, MAX_DENSITY);
        particle->pressure = SPEED_OF_SOUND * (particle->density - MAX_DENSITY);
        particle->updatedPressure = 1;
        delete neighbours;
        return false;
    }
    if (particle->updatedAcceleration != 1) {
        // reset the acceleration, the velocity stays (sph is velocity driven)
        particle->acceleration *= 0.f;
        int kill = 0;
        for (std::list<LockedList>::iterator Lists=neighbours->begin(); Lists!=neighbours->end(); ++Lists) {
            for (std::list<void*>::iterator it=(*Lists)->Particles->begin(); it!=(*Lists)->Particles->end(); ++it) {
                WaterParticle_t neighbourParticle = (WaterParticle_t) (*it);
                float distance = sqrt(powf(neighbourParticle->pos[0] - particle->pos[0], 2) + powf(neighbourParticle->pos[1] - particle->pos[1], 2));
                // this check is so that we don't compare it to itself, which will cause the acceleration
                // to nan, because the distance to itself is v v v v tiny (floats, you know)
                if (particle != neighbourParticle) {
                    //if findacceleration wants particle dead, it gets particle dead
                    if (findAcceleration(particle, neighbourParticle, distance)) {
                        kill = 1;
                    }
                }
            }
            (*Lists)->Lock.unlock();
        }
        //apply gravity as well
        particle->acceleration[1] -= GRAVITY;
        particle->updatedAcceleration = 1;
        delete neighbours;
        if (kill) {
            return true;
        }
        return false;
    }

    glm::vec2 delta_v = particle->prev_velocity;
    particle->prev_velocity += particle->acceleration*time_step;

    // old pos is saved to update the neighbour tree, 
    // and to check if the line between them crosses a rigid body
    glm::vec2 old_pos = particle->pos;

    RigidBodyCheck(particle);
    
    if (particle->prev_velocity.length() > MAX_VELOCITY) {
        particle->prev_velocity = glm::normalize(particle->prev_velocity) * MAX_VELOCITY;
    }

    particle->pos += particle->prev_velocity * time_step;

    particle->velocity = (delta_v + particle->prev_velocity) / 2.0f;

    //time to update pressure and acceleration again
    particle->updatedPressure = 0;
    particle->updatedAcceleration = 0;
    current_time += time_step;

    cellList->move(&old_pos[0], particle, &particle->pos[0]);
    //cellList->assertCorrectness();
    if (DEATH || cellList->isOutOfBounds(&particle->pos[0])) {
        cellList->remove(&particle->pos[0], particle);
        return true;
    }
    return false;
}
