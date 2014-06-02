/*
 * FireHandler.hpp
 *
 *  Created on: 28 maj 2014
 *      Author: Erik
 */

#ifndef FIREHANDLER_HPP_
#define FIREHANDLER_HPP_

#include <ParticleInitializer.h>
#include <ParticleUpdater.h>
#include "FireParticle.hpp"
#include "../particle/Initializers.hpp"
#include "../particle/Updaters.hpp"

class FireHandler : public ParticleInitializer<FireParticle>, public ParticleUpdater<FireParticle> {
	static constexpr unsigned char
	fireStartMin[4] {50,200,200,170},
	firestartMax[4]{100,200,200,200},
	fireColorEnd[3] {0xFF, 0xA2, 0};
	const unsigned char particleAlpha = 50;

	RandomLifetimeInitializer lifetime;
	RandomSizeInitializer size;
	RandomColorInitializer color;
	VelocitySpreadInitializer speed;
	RandomSpherePositionInitializer pos;

	ColorGradient first,second,third,fourth;

	OscilatingPositionOffset offsetter;
	AlphaFader fader, fadeIn;

	float delta, startFade;

	vec3 position;

public:
	vec3 outsideForce;
	FireHandler(float lMin,float lMax, float sMin, float sMax, vec3 pos, float startFade) :
		lifetime(lMin,lMax),
		size(sMin,sMax),
		speed(vec3(0,5,0), .4, 1.3, 2.4),
		pos(pos,.1,1),
		fader(startFade, lMax * 2, 0, false),
		fadeIn(0, startFade, particleAlpha, false) {
		unsigned char
			fireStartMin[4] {255,255,255,110},
			firestartMax[4]{255,255,255,130},
			fireColor1[3] {255, 0xF0, 0x80},
			fireColor2[3] {255, 230, 40},
			fireColor3[3] {200, 100, 10},
			fireColor4[3] {255, 0, 0};

			this->startFade = startFade;
			first = ColorGradient(0,.2,fireColor1);
			second = ColorGradient(.2,.5,fireColor2);
			third = ColorGradient(.5,.9,fireColor3);
			fourth = ColorGradient(.9,.99,fireColor4);
		color = RandomColorInitializer(fireStartMin,firestartMax);
		delta = 0;
		position = pos;
		position.y = 0;
	}

	void setDeltaTime(float d) { delta = d; }

	void initParticle(FireParticle* p) override {
		p->life = lifetime.getRandomLifetime();
		p->startLife = p->life;
		p->actualSize = size.getRandSize();
		p->speed = speed.getRandVelocitySpread();
		p->oscFreq = linearRand<float>(5,17);//5,17
		p->oscRate = linearRand<float>(.02,.3);//.02,.3
		p->pos = pos.getRandomSpherePosition();
		p->size = size.getRandSize();
		p->actualPos = p->pos;
		color.getRandomColor(&p->r);
		p->a = 0;//(unsigned char) linearRand<float>(140, 255);
	}

	float getSize(float life, float size) {
		if(.3 < life && life < .7) {
			float x = life;
			return size + size * 3 * ((x * x) - x + .3 * .7);
		}
		return size;
	}

	float setIntensity(unsigned char* color, float inter, vec3 pos) {
		pos.y = 0;
		float d = glm::distance(position,pos);
		/*for(int i = 0; i < 3; i++) {
			color[i] = (unsigned char)((1-inter) * color[i]);
		}*/
		if(d > .8)
			return std::max<float>((.2 - d + .8)/.2,0);
		else
			return 1;
	}

	bool updateParticle(FireParticle* p) override {
		p->life -= delta;
		if(p->life <= 0) {
			return true;
		}

		float lifetimeInterpolation = (p->startLife - p->life) / p->startLife;
		p->speed += (vec3(0,6,0) + outsideForce) * delta;
		p->actualPos += p->speed * delta;
		//p->pos = p->actualPos;
		p->pos = p->actualPos + offsetter.getOscilatingXPos(p->speed,p->oscFreq,p->startLife - p->life,p->oscRate,0);
		p->a = fadeIn.getAlpha(p);
		//p->a = fader.getAlpha(p);
		if(lifetimeInterpolation > startFade) {
			float inter = (lifetimeInterpolation - startFade) / (1 - startFade);
			p->a = (unsigned char) (particleAlpha * (1 - inter));
		}

		p->size = getSize(lifetimeInterpolation, p->actualSize);
		//p->a *= setIntensity(&p->r,lifetimeInterpolation, p->pos);
		setColors(p);
		p->r = lerp(p->r, lifetimeInterpolation, fireColorEnd[0]);
		p->g = lerp(p->g, lifetimeInterpolation, fireColorEnd[1]);
		p->b = lerp(p->b, lifetimeInterpolation, fireColorEnd[2]);
		return false;
	}

	void setColors(FireParticle* p) {
		float inter = (p->startLife - p->life) / p->startLife;
		first.getColor(&p->r, inter);
		second.getColor(&p->r, inter);
		third.getColor(&p->r, inter);
		fourth.getColor(&p->r, inter);
	}
};


#endif /* FIREHANDLER_HPP_ */
