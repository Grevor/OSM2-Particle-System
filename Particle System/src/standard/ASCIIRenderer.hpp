/*
 * ASCIIRenderer.hpp
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef ASCIIRENDERER_HPP_
#define ASCIIRENDERER_HPP_

#include "../../test/StringBitmap.hpp"
#include "../core/ParticleRenderer.h"

class ASCIIRenderer : public ParticleRenderer<StandardParticle> {
	StringBitmap* bmp;
public:
	ASCIIRenderer(StringBitmap* bmp) {
		this->bmp = bmp;
	}

	void renderParticle(StandardParticle* particle) override {
		//if(particle->render.alpha > 5)
			bmp->drawDot((int)particle->vec.pos[0], (int)particle->vec.pos[1], (char)particle->render.color[0]);
		//else
		//	bmp->drawDot((int)particle->vec.pos[0], (int)particle->vec.pos[1], ' ');
		//bmp->drawString("Mitra",(int)particle->vec.pos[0], (int)particle->vec.pos[1]);
	}
};



#endif /* ASCIIRENDERER_HPP_ */
