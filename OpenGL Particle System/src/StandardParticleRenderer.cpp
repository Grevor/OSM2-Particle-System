/*
 * StandardParticleRenderer.cpp
 *
 *  Created on: 12 maj 2014
 *      Author: David
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "StandardParticleRenderer.h"
#include "shader.hpp"

static const GLfloat g_vertex_buffer_data[3*4] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
			0.5f,  0.5f, 0.0f,
	};

StandardParticleRenderer::StandardParticleRenderer(int maxParticles, Camera* camera)
{
	this->maxParticles = maxParticles;
	this->camera = camera;
	initGLBuffers();
	initGLShaderProgram();
	setTexture(loadDDS("resources\\particle.DDS"));
}

StandardParticleRenderer::StandardParticleRenderer(ParticleSystem<Particle>* particleSystem, Camera* camera) :
	StandardParticleRenderer(particleSystem->getMaxSize(), camera)
{
	this->particleSystem = particleSystem;
/*	this->maxParticles = particleSystem->getMaxSize();
	this->camera = camera;
	initGLBuffers();
	initGLShaderProgram();*/
}

StandardParticleRenderer::~StandardParticleRenderer()
{
	delete[] g_particle_position_size_data;
	delete[] g_particle_color_data;
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(programID);
	//glDeleteTextures(1, &texture);
}

void StandardParticleRenderer::setTexture(GLuint textureHandle) {
	texture = textureHandle;
	if (texture == 0) {
		textureBlend = 1;
	}
	else {
		textureBlend = 0;
	}
}

void StandardParticleRenderer::render() {
	int nParticles = fillGLBuffers();
	render(nParticles);
}

void StandardParticleRenderer::render(int nParticles) {
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(GLfloat) * 4, g_particle_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(GLubyte) * 4, g_particle_color_data);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	glUseProgram(programID);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureID, 0);
	glUniform1f(textureBlendID, textureBlend);

	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 viewProjectionMatrix = camera->getProjectionMatrix() * viewMatrix;
	// Same as the billboards tutorial
	glUniform3f(cameraRightWorldspaceID, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	glUniform3f(cameraUpWorldspaceID   , viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
	glUniformMatrix4fv(viewProjMatrixID, 1, GL_FALSE, &viewProjectionMatrix[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
	);

	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

	// Draw the particles !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nParticles);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

struct ParticleSortStruct {
	float distSq;
	Particle* particle;

	bool operator<(const ParticleSortStruct& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->distSq > that.distSq;
	}
};

int StandardParticleRenderer::fillGLBuffers() {
	ParticleSortStruct particles[maxParticles];
	ParticleIterator<Particle>* iter = particleSystem->getLivingParticles();
	vec3 camPos = camera->getPosition();
	int nParticles = 0;
	while(iter->hasNext()) {
		particles[nParticles].particle = iter->next();
		particles[nParticles].distSq = glm::length2(particles[nParticles].particle->pos - camPos);
		nParticles++;
	}
	std::sort(&particles[0], &particles[nParticles]);
	for (int i = 0; i < nParticles; ++i) {
		Particle p = *(particles[i].particle);
		g_particle_position_size_data[4*i+0] = p.pos.x;
		g_particle_position_size_data[4*i+1] = p.pos.y;
		g_particle_position_size_data[4*i+2] = p.pos.z;

		g_particle_position_size_data[4*i+3] = p.size;

		g_particle_color_data[4*i+0] = p.r;
		g_particle_color_data[4*i+1] = p.g;
		g_particle_color_data[4*i+2] = p.b;
		g_particle_color_data[4*i+3] = p.a;
	}
	delete iter;
	return nParticles;
}

void StandardParticleRenderer::initGLBuffers() {
	g_particle_position_size_data = new GLfloat[maxParticles * 4];
	g_particle_color_data         = new GLubyte[maxParticles * 4];

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void StandardParticleRenderer::initGLShaderProgram() {
	programID = loadShaders(
			"src\\Particle.vertexshader",
			"src\\Particle.fragmentshader" );

	cameraRightWorldspaceID  = glGetUniformLocation(programID, "cameraRightWorldSpace");
	cameraUpWorldspaceID  = glGetUniformLocation(programID, "cameraUpWorldSpace");
	viewProjMatrixID = glGetUniformLocation(programID, "VP");

	textureID = glGetUniformLocation(programID, "textureSampler");
	textureBlendID = glGetUniformLocation(programID, "textureBlend");
}

// Fill the GPU buffer
/*
g_particule_position_size_data[4*particlesCount+0] = p.pos.x;
g_particule_position_size_data[4*particlesCount+1] = p.pos.y;
g_particule_position_size_data[4*particlesCount+2] = p.pos.z;

g_particule_position_size_data[4*particlesCount+3] = p.size;

g_particule_color_data[4*particlesCount+0] = p.r;
g_particule_color_data[4*particlesCount+1] = p.g;
g_particule_color_data[4*particlesCount+2] = p.b;
g_particule_color_data[4*particlesCount+3] = p.a;*/
