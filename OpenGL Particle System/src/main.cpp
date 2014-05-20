//============================================================================
// Name        : OpenGL.cpp
// Author      : Knugen
// Version     :
// Copyright   : Don't steal from the Knaug
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#ifdef MULTI_THREAD
#include <boost/thread/thread.hpp>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "Camera.hpp"
#include "shader.hpp"
#include "StandardParticleRenderer.h"
#include "Particle.hpp"
#include "TimeCurve.hpp"
#include "Updater.hpp"
#include "Initializer.hpp"
#include "NaiveParticlePool.h"
#include "texture.hpp"
#include "particle/ParticleHandler.hpp"
#include "particle/SampleInitializers.hpp"
#include "particle/Updaters.hpp"
#include "attractor/DustSphere.hpp"
#ifdef MULTI_THREAD
#include "ParticleEngine.hpp"
#endif

using namespace glm;
using namespace std;

#define MAX_FPS 60
#define SPAWN_INCREASE 1000

const int maxParticles = 10000;

GLFWwindow* window;
Camera* mainCamera;
StandardParticleRenderer* renderer;
StandardParticleInitializer* init;
DustSphere::AttractorUpdater* attractorUpdater;
TimeCurve* spawnCurve;
vec3 dustSpherePos = {0,20, 0};
float anglePerSec = .6, posPerSec = 8;
float terrainSize = 100;
double lastTime = 0;
#define NUM_TEXTURES 3
GLuint textures[5];
bool willDrawGrid = true;

inline bool keyIsPressed(GLFWwindow* window, int keyCode) {
	return glfwGetKey(window, keyCode) != 0; //== GLFW_PRESS || glfwGetKey(window, keyCode) == GLFW_REPEAT;
}

void getMousePosition(GLFWwindow* win, double* x, double* y) {
	glfwGetCursorPos(win, x,y);
}

void getNormMousePosition(GLFWwindow* win, float* x, float* y) {
	double px, py;
	int w,h;
	getMousePosition(win, &px,&py);
	//glfwGetWindowPos(win, &xx, &yy);

	glfwGetWindowSize(win,&w,&h);

	*x = (float)(px / w);
	*y = (float)(py / h);

}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	double thisTime = glfwGetTime();
	double delta = thisTime - lastTime;

	double angleDelta = anglePerSec * delta;
	double posDelta = posPerSec * delta;
	lastTime = thisTime;
	if(keyIsPressed(window, GLFW_KEY_UP)) {
		mainCamera->move(0,0,0,angleDelta,0);
	}
	if(keyIsPressed(window, GLFW_KEY_DOWN)) {
		mainCamera->move(0,0,0,-angleDelta,0);
	}
	if(keyIsPressed(window, GLFW_KEY_LEFT)) {
		mainCamera->move(0,0,0,0,angleDelta);
	}
	if(keyIsPressed(window, GLFW_KEY_RIGHT)) {
		mainCamera->move(0,0,0,0,-angleDelta);
	}
	if(keyIsPressed(window, GLFW_KEY_SPACE)) {
		mainCamera->move(0,0,posDelta,0,0);
	}
	if(keyIsPressed(window, GLFW_KEY_LEFT_CONTROL)) {
		mainCamera->move(0,0,-posDelta,0,0);
	}
	if(keyIsPressed(window, GLFW_KEY_W)) {
		mainCamera->move(posDelta,0,0,0,0);
	}
	if(keyIsPressed(window, GLFW_KEY_S)) {
		mainCamera->move(-posDelta,0,0,0,0);
	}
	if(keyIsPressed(window, GLFW_KEY_D)) {
		mainCamera->move(0,posDelta,0,0,0);
	}
	if(keyIsPressed(window, GLFW_KEY_A)) {
		mainCamera->move(0,-posDelta,0,0,0);
	}
	if(keyIsPressed(window, GLFW_KEY_0)) {
		renderer->setTexture(textures[0]);
	}
	if(keyIsPressed(window, GLFW_KEY_1)) {
		renderer->setTexture(textures[1]);
	}
	if(keyIsPressed(window, GLFW_KEY_2)) {
		renderer->setTexture(textures[2]);
	}
	if(keyIsPressed(window, GLFW_KEY_3)) {
		renderer->setTexture(textures[3]);
	}
	if(keyIsPressed(window, GLFW_KEY_4)) {
		renderer->setTexture(textures[4]);
	}
	if(keyIsPressed(window, GLFW_KEY_H)) {
		init->setPosition(init->getPosition() + vec3(posDelta,0,0));
	}
	if(keyIsPressed(window, GLFW_KEY_K)) {
		init->setPosition(init->getPosition() + vec3(-posDelta,0,0));
	}
	if(keyIsPressed(window, GLFW_KEY_U)) {
		init->setPosition(init->getPosition() + vec3(0,0,posDelta));
	}
	if(keyIsPressed(window, GLFW_KEY_J)) {
		init->setPosition(init->getPosition() + vec3(0,0,-posDelta));
	}
	double spawnAmountDelta = SPAWN_INCREASE * delta;
	if(keyIsPressed(window, GLFW_KEY_COMMA)) {
		spawnCurve->addIntensity(spawnAmountDelta,spawnAmountDelta/MAX_FPS);
	}
	if(keyIsPressed(window, GLFW_KEY_PERIOD)) {
		spawnCurve->addIntensity(-spawnAmountDelta,-spawnAmountDelta/MAX_FPS);
	}

	if(keyIsPressed(window, GLFW_KEY_Z)) {
		attractorUpdater->setAttractorPower(5);
	}
	if(keyIsPressed(window, GLFW_KEY_X)) {
		attractorUpdater->setAttractorPower(.5);
	}


	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		//attractorUpdater->setAttractorPos(attractorUpdater->getAttractorPos() + getDeltaVector());
		attractorUpdater->setAttractorPower(10);

		float normX,normY;
		getNormMousePosition(window, &normX, &normY);
		vec3 right = mainCamera->getRightVector();
		vec3 up = cross(right, mainCamera->getForwardVector());

		attractorUpdater->setAttractorPos(dustSpherePos + normalize(right) * (normX - .5f) * 5.f - normalize(up) * (normY -.5f) * 5.f);
	} else {
		attractorUpdater->setAttractorPower(0);
	}

	if(keyIsPressed(window, GLFW_KEY_DELETE)) {
		willDrawGrid = !willDrawGrid;
	}
}

void resizeCallback(GLFWwindow* win, int width, int height) {
	glViewport(0,0,width,height);
	mainCamera->setAspectRatio((float)width/height);
}

void drawGrid() {
	glUseProgram(0);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glm::mat4 matrix = mainCamera->getProjectionMatrix();
	glMultMatrixf((const GLfloat*) (&matrix[0][0]));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	matrix = mainCamera->getViewMatrix();
	glMultMatrixf((GLfloat*) (&matrix[0][0]));
	glBegin(GL_LINES);
	glColor4f(1, 1, 1, .4);
	for (float x = -terrainSize; x <= terrainSize; x += 1) {
		glVertex3f(x, 0, terrainSize);
		glVertex3f(x, 0, -terrainSize);
		glVertex3f(terrainSize, 0, x);
		glVertex3f(-terrainSize, 0, x);
	}
	glEnd();
}

int main(void) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int width = mode->width-6;
	int height = mode->height-60;
	mainCamera = new Camera(vec3(0,2,-5), 0, 0, (float)width/height, 45.0f, 100.0f);

	//1 for yellyshroom, 0 for awesome on the attractorPos.y
	attractorUpdater = new DustSphere::AttractorUpdater(dustSpherePos, dustSpherePos, 5);
	attractorUpdater->setAttractorPower(0);

#define DUST_SPHERE_PARTICLES 200000
	unsigned char dustColor[4] = {255,255,255,100};
	ParticleSystem<Particle>* particleSystem3 = new ParticleSystem<Particle>(DUST_SPHERE_PARTICLES,
			new DustSphere::SphereInitializer(dustSpherePos,5,dustColor,.05),
			attractorUpdater,
			new DustSphere::AllAtOnceSpawnCurve(DUST_SPHERE_PARTICLES));



	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Particle System Demo", NULL, NULL);
	if (!window) {
		fprintf( stderr,
				"Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
		return -1;
	}

	//glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowSizeCallback(window, resizeCallback);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the mainCamera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	textures[0] = 0;
	textures[1] = loadDDS("resources\\particle.DDS");
	textures[2] = loadDDS("resources\\Gclef.dds");
	textures[3] = loadDDS("resources\\Death.dds");

	StandardUpdater* updater = new StandardUpdater(mainCamera,glfwGetTime());
	init = new StandardParticleInitializer(vec3(0,0,0), .6);
	ParticlePool<Particle>* pool = new NaiveParticlePool<Particle>(maxParticles);
	spawnCurve = new TimeCurve(1000/60,1000);
	ParticleSystem<Particle>* particleSystem = new ParticleSystem<Particle>(pool,init,updater,spawnCurve,false);
	renderer = new StandardParticleRenderer(particleSystem, mainCamera, textures[1]);


	/*unsigned char minColor[4] = {100,100,100,100};
	unsigned char maxColor[4] = {255,255,255,255};

	ParticleHandler<FountainInitializer, RandomColorInitializer, RandomSizeInitializer,
	RandomLifetimeInitializer,NullInitializer,NullInitializer,ConstantForceUpdater, PhysicsUpdater, LifetimeUpdater>* handler =
			new ParticleHandler<FountainInitializer, RandomColorInitializer, RandomSizeInitializer,
			RandomLifetimeInitializer,NullInitializer,NullInitializer,ConstantForceUpdater, PhysicsUpdater, LifetimeUpdater>(glfwGetTime(),
					FountainInitializer(vec3(0,0,0),vec3(0,10,0),0,.1,.3,5,10),
					RandomColorInitializer(minColor, maxColor),
					RandomSizeInitializer(.3,2),
					RandomLifetimeInitializer(5,5),
					ConstantForceUpdater(vec3(0,-1,0), false),
					PhysicsUpdater(),
					LifetimeUpdater());

	NaiveParticlePool<Particle>* pool2 = new NaiveParticlePool<Particle>(maxParticles * 20);
	ParticleSystem<Particle>* particleSystem2 = new ParticleSystem<Particle>(pool2,handler,handler,spawnCurve, false);
	StandardParticleRenderer* renderer2 = new StandardParticleRenderer(particleSystem2, mainCamera, textures[0]);*/

	StandardParticleRenderer* renderer3 = new StandardParticleRenderer(particleSystem3, mainCamera, textures[1]);

#ifdef MULTI_THREAD
	ParticleEngine* particleEngine = new ParticleEngine();
	particleEngine->addParticleSystem(particleSystem);
	particleEngine->addParticleSystem(particleSystem2);
	particleEngine->addParticleSystem(particlleSystem3);
#endif

	double timePerFrame = 1.0/MAX_FPS;
	double previousTime = glfwGetTime();
	double newTime = 0;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		newTime = glfwGetTime();
		double deltaTime = newTime - previousTime;
		previousTime = newTime;
#ifdef MULTI_THREAD
		if (deltaTime < timePerFrame) {
			boost::this_thread::sleep(boost::posix_time::microseconds((timePerFrame - deltaTime)*1000000));
		}
#endif
		updater->setTime(newTime);
		//handler->updateDelta(newTime);
		spawnCurve->update(deltaTime);
		attractorUpdater->setDeltaTime(deltaTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifndef MULTI_THREAD
		particleSystem->step();
		//particleSystem2->step();
		particleSystem3->step();
		particleSystem->update();
		//particleSystem2->update();
		particleSystem3->update();
#else
		particleEngine->step();
#endif

		if(willDrawGrid) drawGrid();
		renderer->render();
		//renderer2->render();
		renderer3->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		keyCallback(window,0,0,0,0);

	}

	delete mainCamera;
#ifdef MULTI_THREAD
	delete particleEngine;
#endif
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteTextures(NUM_TEXTURES,&textures[1]);
	glfwTerminate();

	return 0;
}
