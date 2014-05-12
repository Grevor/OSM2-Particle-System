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

class StandardParticleRenderer {
	Camera* camera;
	int maxParticles;
	//ParticleSystem<>

	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;

	GLuint programID;
	GLuint cameraRightWorldspaceID;
	GLuint cameraUpWorldspaceID;
	GLuint viewProjMatrixID;
public:
	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;

	StandardParticleRenderer() = delete;
	StandardParticleRenderer(int maxParticles, Camera* camera);
	virtual ~StandardParticleRenderer();
	void render(int nParticles);
};

#endif /* STANDARDPARTICLERENDERER_H_ */
