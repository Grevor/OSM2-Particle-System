/*
 * StandardGLRenderer.hpp
 *
 *  Created on: 9 maj 2014
 */

#ifndef STANDARDGLRENDERER_HPP_
#define STANDARDGLRENDERER_HPP_
#include <GL/gl.h>
#include <GL/glu.h>
#include "../utilities/ParticleRenderer.h"
#include "StandardParticle.hpp"

class StandardGLRenderer : public ParticleRenderer<StandardParticle> {
	GLUquadric* quad;
public:
	StandardGLRenderer() {
		quad = gluNewQuadric();
		//gluQuadricTexture(quad,true);
	}

	void renderParticle(StandardParticle* particle) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef((float)particle->vec.pos[0], (float)particle->vec.pos[1], (float)particle->vec.pos[2]);
		Vector4f col;
		col[0] = particle->render.color[0];
		col[1] = particle->render.color[1];
		col[2] = particle->render.color[2];
		col[3] = (float)particle->render.alpha / (1 << 6);
		glColor4fv((GLfloat*)(&col));
		//glVertex3fv((GLfloat*)particle->vec.pos);
		//glVertex3fv((GLfloat*)particle->vec.pos);
		//glVertex3fv((GLfloat*)particle->vec.pos);
		gluSphere(quad, particle->render.size.norm(), 30, 10);
		glPopMatrix();
	}

	~StandardGLRenderer() {
		gluDeleteQuadric(quad);
	}
};



#endif /* STANDARDGLRENDERER_HPP_ */
