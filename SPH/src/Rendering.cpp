#include "Flags.h"
#include "GLFW/glfw3.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include "../../Particle System/src/ParticleSystem.h"
#include "WaterParticleStruct.h"
#include "WaterParticle.h"
#include <boost/foreach.hpp>

typedef struct camera_t {
   double x, y, zoom; 
   camera_t(void) {
        x = 0.0;
        y = 0.0;
        zoom = 1.0;
   }
} *Camera_t;

Camera_t camera = new camera_t();

float shift = 1.f;
float ctrl = 1.f;

double rotate_y = 0.0;
double rotate_x = 0.0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        INCCURVE->spawn(1);
        shift = ctrl = 1;
        printf("Spawned 1 new particles!\n");
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        INCCURVE->spawn(-1);
        shift = ctrl = 1;
        printf("Despawned 1 new particles!\n");
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        DEBUG = !DEBUG;
        shift = ctrl = 1;
        printf("Debug is: %d\n", DEBUG);
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        VISCOSITY = VISCOSITY * shift * ctrl - 0.1f;
        shift = ctrl = 1;
        printf("Updated VISCOSITY to: %lf\n", VISCOSITY);
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        VISCOSITY = VISCOSITY * shift * ctrl + 0.1f;
        shift = ctrl = 1;
        printf("Updated VISCOSITY to: %lf\n", VISCOSITY);
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        DAMPENING = DAMPENING * shift * ctrl - 0.1f;
        shift = ctrl = 1;
        printf("Updated DAMPENING to: %lf\n", DAMPENING);
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        DAMPENING = DAMPENING * shift * ctrl + 0.1f;
        shift = ctrl = 1;
        printf("Updated DAMPENING to: %lf\n", DAMPENING);
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        TIME_STEP = TIME_STEP * shift * ctrl - 0.1f;
        shift = ctrl = 1;
        printf("Updated TIME_STEP to: %lf\n", TIME_STEP);
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        TIME_STEP = TIME_STEP * shift * ctrl + 0.1f;
        shift = ctrl = 1;
        printf("Updated TIME_STEP to: %lf\n", TIME_STEP);
    }
    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        SPEED_OF_SOUND = SPEED_OF_SOUND * shift * ctrl - 0.1f;
        shift = ctrl = 1;
        printf("Updated SPEED OF SOUND to: %lf\n", SPEED_OF_SOUND);
    }
    if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        SPEED_OF_SOUND = SPEED_OF_SOUND * shift * ctrl + 0.1f;
        shift = ctrl = 1;
        printf("Updated SPEED OF SOUND to: %lf\n", SPEED_OF_SOUND);
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        MAX_DENSITY = MAX_DENSITY * 1.f * shift * ctrl - 0.1f;
        shift = ctrl = 1;
        printf("Updated MAX DENSITY to: %lf\n", MAX_DENSITY);
    }
    if (key == GLFW_KEY_J && action == GLFW_PRESS) {
        MAX_DENSITY = MAX_DENSITY * shift * ctrl + 0.1f;
        shift = ctrl = 1;
        printf("Updated MAX DENSITY to: %lf\n", MAX_DENSITY);
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        WATER_DENSITY = WATER_DENSITY * shift * ctrl + 0.1f;
        shift = ctrl = 1;
        printf("Updated WATER DENSITY to: %lf\n", WATER_DENSITY);
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        WATER_DENSITY = WATER_DENSITY * shift * ctrl - 0.1f;
        shift = ctrl = 1;
        printf("Updated WATER DENSITY to: %lf\n", WATER_DENSITY);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        DEATH = !(DEATH);
        printf("Killing particles, status: %d\n", DEATH);
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        shift *= 10;
    }
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        ctrl = ctrl / 10;
    }

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

void draw_axes() {
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION); //change to Ortho view
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,SYSTEM_WIDTH,0,SYSTEM_HEIGHT);

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(1.1f,1,0);
	//glMultMatrixf(Transform.M);

	glBegin(GL_LINES);
	glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glEnd();

	glPopMatrix();

	//pop GL_PROJECTION
	glMatrixMode(GL_PROJECTION); //change to Pers view
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glEnable(GL_LIGHTING);
}

void renderSystem(ParticleSystem<struct waterParticle_t> *sys) {
    glBegin(GL_POINTS);
    ParticleIterator<struct waterParticle_t> *living = sys->getLivingParticles();
    WaterParticle_t curr;
    while(living->hasNext()) {
        curr = living->next();
        glColor3f(curr->color[0], curr->color[1], curr->color[2]);
        glVertex2f(curr->pos[0], curr->pos[1]);
        living->done(curr);
    }
    delete living;
    glEnd();
}

void renderStatic(std::vector <struct line> sp) {
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    BOOST_FOREACH(const line &v, sp) {
        glVertex2f(v.pos[0], v.pos[1]);
        glVertex2f(v.pos[0] + v.vec[0]*SYSTEM_WIDTH, v.pos[1] + v.vec[1]*SYSTEM_WIDTH);
    }
	glEnd();
}

void renderParticles(ParticleSystem<struct waterParticle_t> *system, std::vector<struct line> static_lines, GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //glTranslated(camera->x, camera->y, camera->zoom);

    //draw_axes();
    //glTranslatef(25, 30, 25);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION); //change to Ortho view
    glPushMatrix();
    glLoadIdentity();
	gluOrtho2D(0,SYSTEM_WIDTH,0,SYSTEM_HEIGHT);

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glPointSize(POINT_SIZE);
    renderSystem(system);
    renderStatic(static_lines);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION); //change to Pers view
    glPopMatrix();

    glMatrixMode( GL_MODELVIEW );

    glfwSwapBuffers(window);
    glfwPollEvents();
}
