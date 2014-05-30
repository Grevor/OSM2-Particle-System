#include "StaticParticle.h"
#include "Flags.h"
#include "cellist/Cellist.h"

StaticParticle::StaticParticle(Cellist *c) {
    //adds a vertical wall
    allParticles = *(new std::vector<WaterParticle_t>);
    this->c = c;
    float x = 2.f;
    for (float y = 10.f; y > 2.f; y -= 0.01f) {
        WaterParticle_t wp = new struct waterParticle_t(); 
        wp->pos = glm::vec2(x,y);
        initParticle(wp);
        c->insert(&wp->pos[0], wp);
        allParticles.push_back(wp);
        x += 0.01f;
    }
    x = 2.f;
    for (float y = 9.95f; y > 2.f; y -= 0.01f) {
        WaterParticle_t wp = new struct waterParticle_t(); 
        wp->pos = glm::vec2(x,y);
        initParticle(wp);
        c->insert(&wp->pos[0], wp);
        allParticles.push_back(wp);
        x += 0.01f;
    }
    //adds a horizontal wall
    float y = 5.f;
    for (float x = 2.f; x < 15.f; x += 0.05f) {
        WaterParticle_t wp = new struct waterParticle_t(); 
        wp->pos = glm::vec2(x,y);
        initParticle(wp);
        c->insert(&wp->pos[0], wp);
        allParticles.push_back(wp);
    }
}

std::vector<WaterParticle_t> StaticParticle::getStaticParticles(void) {
    return allParticles;
}

StaticParticle::~StaticParticle() {}

void StaticParticle::initParticle(WaterParticle_t particle) {
    particle->pressure = STATIC_PARTICLE_PRESSURE;
    particle->density = STATIC_PARTICLE_DENSITY;
    particle->color = glm::vec3(STATIC_PARTICLE_X_COLOR, STATIC_PARTICLE_Y_COLOR, STATIC_PARTICLE_Z_COLOR);
    particle->acceleration = glm::vec2(0, 0);
    particle->velocity = glm::vec2(0, 0);
    particle->isStatic = 1;
}
