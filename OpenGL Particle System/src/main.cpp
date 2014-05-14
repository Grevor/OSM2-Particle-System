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
#ifdef MULTI_THREAD
#include "ParticleEngine.hpp"
#endif

using namespace glm;
using namespace std;

#define MAX_FPS 60
#define SPAWN_INCREASE 1000

const int maxParticles = 10000;
Particle particlesContainer[maxParticles];
int lastUsedParticle = 0;

GLFWwindow* window;
Camera* mainCamera;
StandardParticleRenderer* renderer;
StandardParticleInitializer* init;
TimeCurve* spawnCurve;
float anglePerSec = .6, posPerSec = 8;
float terrainSize = 100;
double lastTime = 0;
#define NUM_TEXTURES 3
GLuint textures[5];

inline bool keyIsPressed(GLFWwindow* window, int keyCode) {
	return glfwGetKey(window, keyCode) != 0; //== GLFW_PRESS || glfwGetKey(window, keyCode) == GLFW_REPEAT;
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
}

void resizeCallback(GLFWwindow* win, int width, int height) {
	glViewport(0,0,width,height);
	mainCamera->setAspectRatio((float)width/height);
}

void drawGrid() {
	//particleEngine->step();
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
	ParticlePool<Particle>* pool = new NaiveParticlePool<Particle>(maxParticles* 20);
	spawnCurve = new TimeCurve(glfwGetTime(),1000/60,1000);
	ParticleSystem<Particle>* particleSystem = new ParticleSystem<Particle>(pool,init,updater,spawnCurve,false);
	renderer = new StandardParticleRenderer(particleSystem, mainCamera, textures[1]);

#ifdef MULTI_THREAD
	ParticleEngine* particleEngine = new ParticleEngine();
	particleEngine->addParticleSystem(particleSystem);
#endif

	//double lastTime = glfwGetTime();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
//		double currentTime = glfwGetTime();
//		double delta = currentTime - lastTime;
//		lastTime = currentTime;
		updater->setTime(glfwGetTime());

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifndef MULTI_THREAD
		particleSystem->step();
		particleSystem->update();
#else
		particleEngine->step();
#endif

		drawGrid();
		renderer->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		keyCallback(window,0,0,0,0);
	}

	delete mainCamera;
	delete particleEngine;
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteTextures(NUM_TEXTURES,&textures[1]);
	glfwTerminate();

	return 0;
}
