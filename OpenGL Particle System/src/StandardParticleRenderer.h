/*
 * StandardParticleRenderer.h
 *
 *  Created on: 12 maj 2014
 */

#ifndef STANDARDPARTICLERENDERER_H_
#define STANDARDPARTICLERENDERER_H_

#include <list>

#include <GL/gl.h>
#include "Camera.hpp"
#include "Particle.hpp"
#include "ParticleSystem.hpp"
#include "texture.hpp"
#include "RenderableParticleSystem.hpp"

#define MAX_MATERIALS 8
#define MAX_RENDER_PASS_N_PARTICLES (1<<16)

struct ParticleSortStruct {
	float camSpaceZ;
	Particle* particle;
	int index;

	bool operator<(const ParticleSortStruct& that) const {
		return this->camSpaceZ < that.camSpaceZ;
	}
};

class StandardParticleRenderer {


	Camera* camera;
	int maxParticles;
	//ParticleSystem<Particle>* particleSystem;
	std::list<RenderableParticleSystem*> particleSystems;
	std::list<ParticleIterator<Particle>*> iteratorToDelete;

	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	GLuint particles_index_buffer;
	GLuint particle_animation_buffer;

	GLfloat* g_particle_position_size_data;
	GLubyte* g_particle_color_data;
	GLfloat* g_particle_animation_frame_data;
	GLuint*	 g_particle_index_data;

	GLuint programID;
	GLuint cameraRightWorldspaceID;
	GLuint cameraUpWorldspaceID;
	GLuint viewProjMatrixID;
	GLuint textureSamplerID;
	GLuint textureBlendID;
	GLuint textureAnimationFrameFactorID;

	bool texturesToBind[MAX_MATERIALS] = {false};
	GLuint texture[MAX_MATERIALS] = {0};
	GLfloat textureBlend[MAX_MATERIALS] = {1}; //0 = texture, 1 = no texture
	GLfloat textureAnimationFrameFactor[MAX_MATERIALS] = {1};


public:
	StandardParticleRenderer() = delete;
	StandardParticleRenderer(Camera* camera);
	//StandardParticleRenderer(ParticleSystem<Particle>* particleSystem, Camera* camera);
	//StandardParticleRenderer(ParticleSystem<Particle>* particleSystem, Camera* camera, GLuint texture);
	virtual ~StandardParticleRenderer();
	void addParticleSystem(RenderableParticleSystem* particleSystem);
	void setMaterial(ParticleMaterial* material, int index);

	//void setTexture(GLuint textureHandle);
	/**
	 * Fills gpu buffers with particles from the currently set particle system and renders them.
	 */
	void render();

private:
	/**
	 * Render a given number of particles in prefilled buffers.
	 * @param nParticles is the number of particles to from the buffers.
	 */
	void render(int nParticles);
	int fetchAndSortParticles(std::vector<ParticleSortStruct> & dest);
	void fillGLBuffers(std::vector<ParticleSortStruct> & src, int offset, int fillAmount);
	void initGLBuffers();
	void initGLShaderProgram();
	void init(Camera* camera);
	void bindTextures();
};

#endif /* STANDARDPARTICLERENDERER_H_ */
