/*
 * WaterParticle.h
 *
 * Created on: 29 apr 2014
 *  Author: andreas
 */

#define density 1000
#define speed_of_sound 340

#ifndef WATERPARTICLE_H_
#define WATERPARTICLE_H_
#include "../../Particle System/src/ParticleUpdater.h"
#include "WaterParticle.h"

class WaterParticleUpdater : public ParticleUpdater<struct waterParticle> {
public:
    WaterParticleUpdater() {}

    bool updateParticle(struct waterParticle* particle) override {}

    ~WaterParticleUpdater() override {}
};

#endif /* WATERPARTICLE_H_ */
