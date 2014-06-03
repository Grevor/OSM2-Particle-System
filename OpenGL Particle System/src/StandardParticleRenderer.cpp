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

void StandardParticleRenderer::init(Camera* camera) {
	this->maxParticles = 0;
	this->camera = camera;
	initGLBuffers();
	initGLShaderProgram();
	//setTexture(texture);
}

StandardParticleRenderer::StandardParticleRenderer(Camera* camera) {
	init(camera);
}

/*StandardParticleRenderer::StandardParticleRenderer(ParticleSystem<Particle>* particleSystem, Camera* camera)
{
	init(particleSystem->getMaxSize(), camera, 0);
	this->particleSystem = particleSystem;
}*/

/*StandardParticleRenderer::StandardParticleRenderer(ParticleSystem<Particle>* particleSystem, Camera* camera, GLuint texture) :
	StandardParticleRenderer(particleSystem,camera)
{
	setTexture(texture);
}*/

StandardParticleRenderer::~StandardParticleRenderer()
{
	delete[] g_particle_position_size_data;
	delete[] g_particle_color_data;
	delete[] g_particle_animation_frame_data;
	delete[] g_particle_index_data;
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteBuffers(1, &particle_animation_buffer);
	glDeleteBuffers(1, &particles_index_buffer);
	glDeleteProgram(programID);
}

void StandardParticleRenderer::addParticleSystem(RenderableParticleSystem* particleSystem) {
	particleSystems.push_front(particleSystem);
	maxParticles += particleSystem->getParticleSystem()->getMaxSize();
}

void StandardParticleRenderer::setMaterial(ParticleMaterial* material, int index) {
	if (index < 0 || MAX_MATERIALS <= index) {
		throw std::invalid_argument("Material index not in valid range.");
	}
	texture[index] = material->getTexture();
	textureBlend[index] = material->getTextureBlend();
	textureAnimationFrameFactor[index] = material->getAnimationFrameFactor();
}

void StandardParticleRenderer::render(int nParticles) {
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(GLfloat) * 4, g_particle_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(GLubyte) * 4, g_particle_color_data);

	glBindBuffer(GL_ARRAY_BUFFER, particle_animation_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(GLfloat), g_particle_animation_frame_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_index_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * sizeof(GLuint), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(GLuint), g_particle_index_data);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	glUseProgram(programID);

	// Bind our texture in Texture Unit 0
	//glActiveTexture(GL_TEXTURE0); //GL_TEXTURE_2D_ARRAY
	//glBindTexture(GL_TEXTURE_2D, texture[0]);
	//bindTextures();

	const GLint temp[] = {0,1,2,3,4,5,6,7};//,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
	glUniform1iv(textureSamplerID, MAX_MATERIALS, temp);
	glUniform1fv(textureBlendID, MAX_MATERIALS, textureBlend);
	glUniform1fv(textureAnimationFrameFactorID, MAX_MATERIALS, textureAnimationFrameFactor);

	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 viewProjectionMatrix = camera->getProjectionMatrix() * viewMatrix;

	glUniform3f(cameraRightWorldspaceID, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	glUniform3f(cameraUpWorldspaceID   , viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
	glUniformMatrix4fv(viewProjMatrixID, 1, GL_FALSE, &viewProjectionMatrix[0][0]);

	// 1st attribute buffer : vertices
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
			2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
	);

	// 4th attribute buffer : particles' animation frame
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, particle_animation_buffer);
	glVertexAttribPointer(
			3,                                // attribute. No particular reason for 3, but must match the layout in the shader.
			1,                                // size
			GL_FLOAT,                 		  // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
	);

	// 5th attribute buffer: particles index
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, particles_index_buffer);
	glVertexAttribPointer(
			4,                                // attribute. No particular reason for 4, but must match the layout in the shader.
			1,                                // size
			GL_UNSIGNED_INT,           		  // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
	);

	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)
	glVertexAttribDivisor(2, 1); // color : one per quad
	glVertexAttribDivisor(3, 1); // animation frame: one per quad.
	glVertexAttribDivisor(4, 1); // index: one per quad.

	// Draw the particles !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nParticles);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

void StandardParticleRenderer::render() {
	std::vector<ParticleSortStruct> particles (maxParticles);
	int nParticles = fetchAndSortParticles(particles);
	bindTextures();
	for (int offset = 0; offset < nParticles; offset += MAX_RENDER_PASS_N_PARTICLES) {
		int fillAmount = nParticles - offset;
		if (fillAmount > MAX_RENDER_PASS_N_PARTICLES)
			fillAmount = MAX_RENDER_PASS_N_PARTICLES;
		fillGLBuffers(particles, offset, fillAmount);
		render(fillAmount);
	}
	while(!iteratorToDelete.empty()) {
		delete iteratorToDelete.front();
		iteratorToDelete.pop_front();
	}
	//delete particles;
	//free(particles);
}

int StandardParticleRenderer::fetchAndSortParticles(std::vector<ParticleSortStruct> & dest) {
	mat4 viewMatrix = camera->getViewMatrix();
	vec3 viewMatrixZRow(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
	float viewMatrixZTranslation = viewMatrix[3][2];
	int nParticles = 0;
	std::list<RenderableParticleSystem*>::iterator psIter = particleSystems.begin();
	for (int i = 0; i < MAX_MATERIALS; i++) {
		texturesToBind[i] = false;
	}
	while (psIter != particleSystems.end()) {
		//TODO: performance: check if the particle system is within the view frustum
		texturesToBind[(*psIter)->getMaterialIndex()] = true;
		ParticleIterator<Particle>* iter = (*psIter)->getParticleSystem()->getLivingParticles();
		while(iter->hasNext()) {
			Particle* p = iter->next();
			if (p == NULL) {
				break;
			}
			dest[nParticles].particle = p;
			dest[nParticles].camSpaceZ = glm::dot(dest[nParticles].particle->pos, viewMatrixZRow) + viewMatrixZTranslation;
			dest[nParticles].index = (*psIter)->getMaterialIndex();
			nParticles++;
		}
		iteratorToDelete.push_back(iter);
		psIter++;
	}
	std::sort(dest.begin(), dest.begin()+nParticles);
	//std::sort(dest.begin(), dest.end());
	//std::sort(&dest[0], &dest[nParticles]);
	return nParticles;
}

void StandardParticleRenderer::fillGLBuffers(std::vector<ParticleSortStruct> & src, int offset, int fillAmount) {
	for (int i = 0; i < fillAmount; ++i) {
		Particle* p = src[offset+i].particle;
		g_particle_position_size_data[4*i+0] = p->pos.x;
		g_particle_position_size_data[4*i+1] = p->pos.y;
		g_particle_position_size_data[4*i+2] = p->pos.z;

		g_particle_position_size_data[4*i+3] = p->size;

		g_particle_color_data[4*i+0] = p->r;
		g_particle_color_data[4*i+1] = p->g;
		g_particle_color_data[4*i+2] = p->b;
		g_particle_color_data[4*i+3] = p->a;

		g_particle_animation_frame_data[i] = p->animationFrame;

		g_particle_index_data[i] = src[offset+i].index;
	}
}

void StandardParticleRenderer::initGLBuffers() {
	g_particle_position_size_data = new GLfloat[MAX_RENDER_PASS_N_PARTICLES * 4];
	g_particle_color_data         = new GLubyte[MAX_RENDER_PASS_N_PARTICLES * 4];
	g_particle_animation_frame_data = new GLfloat[MAX_RENDER_PASS_N_PARTICLES];
	g_particle_index_data			= new GLuint[MAX_RENDER_PASS_N_PARTICLES];

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particle_animation_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particle_animation_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particles_index_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_index_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_PASS_N_PARTICLES * sizeof(GLuint), NULL, GL_STREAM_DRAW);
}

void StandardParticleRenderer::initGLShaderProgram() {
	programID = loadShaders(
			"src\\Particle.vertexshader",
			"src\\Particle.fragmentshader" );

	cameraRightWorldspaceID  = glGetUniformLocation(programID, "cameraRightWorldSpace");
	cameraUpWorldspaceID  = glGetUniformLocation(programID, "cameraUpWorldSpace");
	viewProjMatrixID = glGetUniformLocation(programID, "VP");

	textureSamplerID = glGetUniformLocation(programID, "textureSampler");
	textureBlendID = glGetUniformLocation(programID, "textureBlend");
	textureAnimationFrameFactorID = glGetUniformLocation(programID, "textureFrameFactor");
}

void StandardParticleRenderer::bindTextures() {
	for (int i = 0; i < MAX_MATERIALS; i++) {
		//if (texturesToBind[i]) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texture[i]);
		//}
	}
	glActiveTexture(GL_TEXTURE0);
}
