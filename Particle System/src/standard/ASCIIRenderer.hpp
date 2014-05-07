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
		bmp->drawDot((int)particle->vec.pos[0], (int)particle->vec.pos[1], '*');
	}
};



#endif /* ASCIIRENDERER_HPP_ */
