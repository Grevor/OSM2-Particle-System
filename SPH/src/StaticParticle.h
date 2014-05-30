/*
 * StaticParticle.h
 *
 *  Created on: 16 maj 2014
 *      Author: andreas 
 */

#ifndef STATICPARTICLE_H_
#define STATICPARTICLE_H_

#include "WaterParticleStruct.h"
#include "../../Particle System/src/ParticleUpdater.h"
#include "../../Particle System/src/ParticleInitializer.h"
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "cellist/Cellist.h"

class StaticParticle {
private:
    Cellist *c;
    std::vector<WaterParticle_t> allParticles;
public:
    void initParticle(WaterParticle_t particle);

    StaticParticle(Cellist *c);

    std::vector<WaterParticle_t> getStaticParticles(void);

    ~StaticParticle();
};

#endif
