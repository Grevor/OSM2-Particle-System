#include "WaterParticle.h"
#include "WaterParticleStruct.h"
#include "../../Particle System/src/ParticleSystem.h"
#include "../../Particle System/src/NaiveParticlePool.h"
#include "../../Particle System/src/ConstantCurve.h"
#include "GLFW/glfw3.h"
#include "Flags.h"

using namespace Curves;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

GLFWwindow *initWindow(int x, int y) {
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(x, y, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

void renderParticle(WaterParticle_t particle, GLFWwindow *window) {
    glColor3f(0.f, 0.f, 1.f);
    glVertex3d(particle->pos[0], particle->pos[1], particle->pos[2]);
    return;
}

void renderParticles(ParticleSystem<struct waterParticle_t> *system, GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPerspective(0, width, 0, height, NEAR, FAR);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotated(0, 1.0, 0.0, 0.0);
    glRotated(0, 0.0, 1.0, 0.0);
    glRotated(0, 0.0, 0.0, 1.0);
    glTranslatef(25, 50, 75);
    
    glBegin(GL_QUADS);

    ParticleIterator<struct waterParticle_t> *living = system->getLivingParticles();
    WaterParticle_t curr;
    while(living->hasNext()) {
        curr = living->next();
        renderParticle(curr, window);
        living->done(curr);
    }
	delete living;
    glEnd();
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(void) {

    GLFWwindow *window = initWindow(WINDOW_X, WINDOW_Y);

	NaiveParticlePool<struct waterParticle_t>* pool = new NaiveParticlePool<struct waterParticle_t>(NUM_WATER_PARTICLES);
    WaterParticle *wp = new WaterParticle(NUM_WATER_PARTICLES, VOLUME, WATER_DENSITY, TIME_STEP);
    
	Curve<long,long>* c = new ConstantCurve(NUM_WATER_PARTICLES);

	ParticleSystem<struct waterParticle_t>* system = new ParticleSystem<struct waterParticle_t>(pool,wp,wp,c,false);

    while (!glfwWindowShouldClose(window)) {
        system->step();
        system->update();
        renderParticles(system, window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
