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

    printf("Hi!\n");
    printf("Keyboard shortcuts:\n");
    printf("Use ARROW UP to start spawning particles and DOWN ARROW to stop\n");
    printf("Press \"r\" to start killing particles, and \"r\" again to stop\n");
    printf("\"y\" to decrease the variable SPEED OF SOUND, and \"u\" to increase\n");
    printf("\"h\" to decrease the variable MAX_DENSITY, and \"j\" to increase\n");
    printf("\"q\" to decrease the variable DAMPENING, and \"w\" to increase\n");
    printf("\"a\" to decrease the variable VISCOSITY, and \"s\" to increase\n");
    printf("If you press CTRL before a button, you divide the change by 10\n");
    printf("If you press Shift before a button, you multiply the change by 10\n");
    printf("Example: If viscosity is 0.2, and you press Shift, then a, it becomes 2.1 (2 + 0.1), rather than 0.3 (0.2 + 0.1).\n");
    

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
