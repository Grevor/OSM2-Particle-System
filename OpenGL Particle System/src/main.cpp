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
//#include "ParticleEngine.h"
#include "texture.hpp"

using namespace glm;
using namespace std;

const int maxParticles = 10000;
Particle particlesContainer[maxParticles];
int lastUsedParticle = 0;

GLFWwindow* window;
Camera* mainCamera;
StandardParticleRenderer* renderer;
StandardParticleInitializer* init;
TimeCurve* spawnCurve;
float angleDelta = .1, posDelta = .5;
float terrainSize = 100;
#define NUM_TEXTURES 3
GLuint textures[5];


static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch(key) {
	case GLFW_KEY_UP:
		mainCamera->move(0,0,0,angleDelta,0);
		break;
	case GLFW_KEY_DOWN:
		mainCamera->move(0,0,0,-angleDelta,0);
		break;
	case GLFW_KEY_LEFT:
		mainCamera->move(0,0,0,0,angleDelta);
		break;
	case GLFW_KEY_RIGHT:
		mainCamera->move(0,0,0,0,-angleDelta);
		break;
	case GLFW_KEY_SPACE:
		mainCamera->move(0,0,posDelta,0,0);
		break;
	case GLFW_KEY_LEFT_CONTROL:
		mainCamera->move(0,0,-posDelta,0,0);
		break;
	case GLFW_KEY_W:
		mainCamera->move(posDelta,0,0,0,0);
		break;
	case GLFW_KEY_S:
		mainCamera->move(-posDelta,0,0,0,0);
		break;
	case GLFW_KEY_D:
		mainCamera->move(0,posDelta,0,0,0);
		break;
	case GLFW_KEY_A:
		mainCamera->move(0,-posDelta,0,0,0);
		break;
	case GLFW_KEY_0:
		renderer->setTexture(textures[0]);
		break;
	case GLFW_KEY_1:
		renderer->setTexture(textures[1]);
		break;
	case GLFW_KEY_2:
		renderer->setTexture(textures[2]);
		break;
	case GLFW_KEY_3:
		renderer->setTexture(textures[3]);
		break;
	case GLFW_KEY_4:
		renderer->setTexture(textures[4]);
		break;
	case GLFW_KEY_H:
		init->setPosition(init->getPosition() + vec3(posDelta,0,0));
		break;
	case GLFW_KEY_K:
		init->setPosition(init->getPosition() + vec3(-posDelta,0,0));
		break;
	case GLFW_KEY_U:
		init->setPosition(init->getPosition() + vec3(0,0,posDelta));
		break;
	case GLFW_KEY_J:
		init->setPosition(init->getPosition() + vec3(0,0,-posDelta));
		break;
	case GLFW_KEY_PAGE_UP:
		spawnCurve->addIntensity(1000,1000/60);
		break;
	case GLFW_KEY_PAGE_DOWN:
		spawnCurve->addIntensity(-1000,-1000/60);
		break;
	default:
		break;
	}
}

void resizeCallback(GLFWwindow* win, int width, int height) {
	glViewport(0,0,width,height);
	mainCamera->setAspectRatio((float)width/height);
}

int findUnusedParticle(){
	for(int i=lastUsedParticle; i<maxParticles; i++){
		if (particlesContainer[i].life < 0){
			lastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<lastUsedParticle; i++){
		if (particlesContainer[i].life < 0){
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void sortParticles(){
	std::sort(&particlesContainer[0], &particlesContainer[maxParticles]);
}

void spawnParticles (double delta) {
	int spawnAmount = (int)(delta*1000.0);
	if (spawnAmount > (int)(0.016f*1000.0))
		spawnAmount = (int)(0.016f*1000.0);

	for(int i=0; i<spawnAmount; i++){
		int particleIndex = findUnusedParticle();
		particlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
		particlesContainer[particleIndex].pos = glm::vec3(0,0,0);

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		// Very bad way to generate a random direction;
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		glm::vec3 randomdir = glm::vec3(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f
		);

		particlesContainer[particleIndex].speed = maindir + randomdir*spread;

		// Very bad way to generate a random color
		particlesContainer[particleIndex].r = rand() % 256;
		particlesContainer[particleIndex].g = rand() % 256;
		particlesContainer[particleIndex].b = rand() % 256;
		particlesContainer[particleIndex].a = (rand() % 256) / 3;

		particlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
	}
}

int updateParticles(double delta, vec3 cameraPosition,
		GLfloat* g_particule_position_size_data, GLubyte* g_particule_color_data) {
	// Simulate all particles
	int particlesCount = 0;
	for(int i=0; i<maxParticles; i++){
		Particle& p = particlesContainer[i]; // shortcut
		if(p.life > 0.0f){
			// Decrease life
			p.life -= delta;
			if (p.life > 0.0f){
				// Simulate simple physics : gravity only, no collisions
				p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
				p.pos += p.speed * (float)delta;
				p.cameradistance = glm::length2( p.pos - cameraPosition );
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				g_particule_position_size_data[4*particlesCount+0] = p.pos.x;
				g_particule_position_size_data[4*particlesCount+1] = p.pos.y;
				g_particule_position_size_data[4*particlesCount+2] = p.pos.z;

				g_particule_position_size_data[4*particlesCount+3] = p.size;

				g_particule_color_data[4*particlesCount+0] = p.r;
				g_particule_color_data[4*particlesCount+1] = p.g;
				g_particule_color_data[4*particlesCount+2] = p.b;
				g_particule_color_data[4*particlesCount+3] = p.a;
			}
			else{
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}
			particlesCount++;
		}
	}
	return particlesCount;
}

void initParticles() {
	for (int i = 0; i < maxParticles; i++) {
		particlesContainer[i].life = -1.0f;
		particlesContainer[i].cameradistance = -1.0f;
	}
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

	glfwSetKeyCallback(window, keyCallback);
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

	initParticles();

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

	//ParticleEngine* particleEngine = new ParticleEngine();
	//particleEngine->addParticleSystem(particleSystem);

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

		particleSystem->step();
		particleSystem->update();

		//particleEngine->step();

		drawGrid();
		renderer->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete mainCamera;
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteTextures(NUM_TEXTURES,&textures[1]);
	glfwTerminate();

	return 0;
}
