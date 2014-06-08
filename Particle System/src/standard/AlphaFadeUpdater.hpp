/*
 * AlphaFadeUpdater.hpp
 *
 *  Created on: 5 maj 2014
 */

#ifndef ALPHAFADER_HPP_
#define ALPHAFADER_HPP_

#include "../ParticleUpdater.h"

class AlphaFader : public ParticleUpdater<StandardParticle> {
	bool fadeIn, fadeOut;
	int fadeAlpha;

public:
	AlphaFader(bool fadeIn, bool fadeOut) : AlphaFader(fadeIn,fadeOut,0) { }

	AlphaFader(bool fadeIn, bool fadeOut, int fadeAlpha) {
		this->fadeAlpha = fadeAlpha;
		this->fadeIn = fadeIn;
		this->fadeOut = fadeOut;
	}

	bool updateParticle(StandardParticle* particle) override {
		if(fadeIn && particle->lifetime.fadingIn()) {
			if(fadeOut && particle->lifetime.fadingOut()) {
				float fadeInInterpolation = particle->lifetime.getInterpolationValueFadeIn();
				float fadeOutInterpolation = particle->lifetime.getInterpolationValueFadeOut();
				unsigned char currentAlpha = particle->render.alpha;
				if(fadeInInterpolation > fadeOutInterpolation)
					particle->render.alpha = (unsigned char)lerp(currentAlpha, particle->render.maxAlpha, fadeInInterpolation);
				else
					particle->render.alpha = (unsigned char)lerp(currentAlpha, particle->render.minAlpha, fadeOutInterpolation);
			} else {
				float interpolation = particle->lifetime.getInterpolationValueFadeIn();
				particle->render.alpha =
						(unsigned char)lerp(particle->render.alpha, particle->render.maxAlpha, interpolation);
			}
		} else if(fadeOut && particle->lifetime.fadingOut()) {
			float interpolation = particle->lifetime.getInterpolationValueFadeOut();
			particle->render.alpha =
					(unsigned char)lerp(particle->render.alpha, particle->render.minAlpha, interpolation);
		} else {
			particle->render.alpha = particle->render.maxAlpha;
		}
		return false;
	}

	float lerp(float val, float target, float t) {
		return (1-t) * val + t * target;
	}
};

#endif /* ALPHAFADER_HPP_ */
