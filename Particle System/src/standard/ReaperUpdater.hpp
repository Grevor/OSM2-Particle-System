/*
 * ReaperUpdater.hpp
 *
 *  Created on: 7 maj 2014
 */

#ifndef REAPERUPDATER_HPP_
#define REAPERUPDATER_HPP_

#include "../utilities/Reaper.h"
#include "../ParticleUpdater.h"
#include "StandardParticle.hpp"

class ReaperUpdater : public ParticleUpdater<StandardParticle> {
	Reaper<StandardParticle>* reaper;
public:
	ReaperUpdater(Reaper<StandardParticle>* reaper) {
		this->reaper = reaper;
	}

	bool updateParticle(StandardParticle* particle) {
		return reaper->isDead(particle);
	}
};


#endif /* REAPERUPDATER_HPP_ */
