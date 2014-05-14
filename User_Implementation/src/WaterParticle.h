/*
 * WaterParticle.h
 *
 *  Created on: 07 maj 2014
 *      Author: andreas 
 */

#ifndef WATERPARTICLE_H_ 
#define WATERPARTICLE_H_ 

#include "../../Particle System/src/ParticleInitializer.h"
#include "../../Particle System/src/ParticleUpdater.h"
#include "WaterParticleStruct.h"
#include "../../Concurrent Collections/src/NeighbourTree.h"

class WaterParticle : public ParticleInitializer<struct waterParticle_t>, public ParticleUpdater<struct waterParticle_t> {
private:
    NeighbourTree *tree;
    int particleMass;

    double time_step;
    double current_time;

public:
	void initParticle(WaterParticle_t particle) override;

    bool updateParticle(WaterParticle_t particle) override;

    void findAcceleration(WaterParticle_t main, WaterParticle_t neighbour, double distance);

    double getTime(void);

    void RigidBodyCheck(WaterParticle_t particle);

    WaterParticle(int amountOfParticles, int volume, int density, double time_step);

	~WaterParticle() override;
};

#endif
