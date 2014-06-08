#include "WaterParticle.h"
#include "WaterParticleStruct.h"
#include "../../Particle System/src/ParticleSystem.hpp"
#include "../../Particle System/src/NaiveParticlePool.hpp"
#include "../../Particle System/src/ParticleEngine.hpp"
#include "../../Particle System/src/curves/ConstantCurve.hpp"
#include "Flags.h"
#include "Rendering.cpp"
#include "IncCurve.h"
#include "cellist/Cellist.h"

#include <ctime>
#include <cstdlib>

using namespace Curves;

IncCurve *INCCURVE;
bool DEBUG;
float TIME_STEP;
float WATER_DENSITY;
float SPEED_OF_SOUND;
float MAX_DENSITY;

int main(void) {
    srand (static_cast <unsigned> (time(0)));

    // Initialize our flags
    TIME_STEP = INIT_TIME_STEP;
    H = INIT_H;
    HH = H * H;
    VISCOSITY = INIT_VISCOSITY;
    DAMPENING = INIT_DAMPENING;
    DEBUG = false;
	INCCURVE = new IncCurve();
    WATER_DENSITY = INIT_WATER_DENSITY;
    SPEED_OF_SOUND = INIT_SPEED_OF_SOUND;
    MAX_DENSITY = INIT_MAX_DENSITY;

    // ConstantCurve for the static particles
    Cellist *c = new Cellist(SYSTEM_WIDTH, SYSTEM_HEIGHT, CELL_SIZE);

	NaiveParticlePool<struct waterParticle_t>* pool = new NaiveParticlePool<struct waterParticle_t>(NUM_WATER_PARTICLES);
    WaterParticle *wp = new WaterParticle(NUM_WATER_PARTICLES, VOLUME, WATER_DENSITY, TIME_STEP, c);

	ParticleSystem<struct waterParticle_t>* system = new ParticleSystem<struct waterParticle_t>(pool,wp,wp,INCCURVE,false);

    ParticleEngine *particleEngine = new ParticleEngine();
    particleEngine->addParticleSystem(system);

    GLFWwindow *window = initWindow(WINDOW_X, WINDOW_Y);

    while (!glfwWindowShouldClose(window)) {
        particleEngine->step();
        particleEngine->waitStepComplete();
        renderParticles(system, wp->getLines(), window);
        //c->assertCorrectness();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
