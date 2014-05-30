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
#include "cellist/Cellist.h"

typedef struct line {
    line(glm::vec2 pos, glm::vec2 vec) {
        this->pos = pos;
        this->vec = vec;
        this->normal = glm::normalize(glm::vec2(-1.f*vec[1], vec[0]));
    }
    glm::vec2 pos;
    glm::vec2 vec;
    glm::vec2 normal;
} *Line;

class WaterParticle : public ParticleInitializer<struct waterParticle_t>, public ParticleUpdater<struct waterParticle_t> {
private:
    Cellist *cellList;
    float particleMass;

    float time_step;
    float current_time;

    std::vector<struct line> lines;
public:
	void initParticle(WaterParticle_t particle) override;

    bool updateParticle(WaterParticle_t particle) override;

    bool findAcceleration(WaterParticle_t main, WaterParticle_t neighbour, float distance);

    float getTime(void);

    void initLines(void);

    std::vector<struct line> getLines(void);

    void RigidBodyCheck(WaterParticle_t particle);

    WaterParticle(int amountOfParticles, float volume, float density, float time_step, Cellist *c);

	~WaterParticle() override;
};

#endif
