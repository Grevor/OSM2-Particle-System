/*
 * StandardParticleRenderer.h
 *
 *  Created on: 12 maj 2014
 *      Author: David
 */

#ifndef STANDARDPARTICLERENDERER_H_
#define STANDARDPARTICLERENDERER_H_

//#include <src/core/ParticleRenderer.h>
#include <GL/gl.h>
#include "Camera.hpp"
#include "Particle.hpp"
#include "ParticleSystem.h"
#include "texture.hpp"

class StandardParticleRenderer {
	Camera* camera;
	int maxParticles;
	ParticleSystem<Particle>* particleSystem;

	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;

	GLuint programID;
	GLuint cameraRightWorldspaceID;
	GLuint cameraUpWorldspaceID;
	GLuint viewProjMatrixID;

	GLuint texture;
	GLuint textureID;
	GLfloat textureBlend; //0 = texture, 1 = no texture
	GLuint textureBlendID;
public:
	GLfloat* g_particle_position_size_data;
	GLubyte* g_particle_color_data;

	StandardParticleRenderer() = delete;
	StandardParticleRenderer(int maxParticles, Camera* camera);
	StandardParticleRenderer(ParticleSystem<Particle>* particleSystem, Camera* camera);
	virtual ~StandardParticleRenderer();
	void setTexture(GLuint textureHandle);
	/**
	 * Fills gpu buffers with particles from the currently set particle system and renders them.
	 */
	void render();
	/**
	 * Render a given number of particles in prefilled buffers.
	 * @param nParticles is the number of particles to from the buffers.
	 */
	void render(int nParticles);

private:
	int fillGLBuffers();
	void initGLBuffers();
	void initGLShaderProgram();
};

#endif /* STANDARDPARTICLERENDERER_H_ */
