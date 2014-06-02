/*
 * ParticleMaterial.hpp
 *
 *  Created on: 20 maj 2014
 *      Author: David
 */

#ifndef PARTICLEMATERIAL_HPP_
#define PARTICLEMATERIAL_HPP_

/**
 * Contains information for how a particle should be rendered
 */
class ParticleMaterial {
	GLuint texture;
	GLfloat textureBlend;
	GLfloat animationFrameFactor;
public:
	ParticleMaterial() {
		setTexture(0);
	}

	ParticleMaterial(GLuint textureHandle) {
		setTexture(textureHandle);
	}

	GLuint getTexture() {
		return texture;
	}

	GLfloat getTextureBlend() {
		return textureBlend;
	}

	GLfloat getAnimationFrameFactor() {
		return animationFrameFactor;
	}

	void setTexture(GLuint textureHandle) {
		setAnimatedTexture(textureHandle, 1);
	}

	void setAnimatedTexture(GLuint textureHandle, int nFrames) {
		this->texture = textureHandle;
		if (texture == 0) {
			textureBlend = 1;
		}
		else {
			textureBlend = 0;
		}
		setNumTextureFrames(nFrames);
	}

	void setNumTextureFrames(int frames) {
		if(frames == 0) {
			frames = 1;
		}
		animationFrameFactor = 1.0 / frames;
	}
};

#endif /* PARTICLEMATERIAL_HPP_ */
