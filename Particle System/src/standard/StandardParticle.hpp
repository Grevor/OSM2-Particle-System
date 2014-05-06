/*
 * FireParticle.h
 *
 *  Created on: 1 maj 2014
 *      Author: Erik
 */

#ifndef FIREPARTICLE_H_
#define FIREPARTICLE_H_

#include <Eigen/Eigen>
#include "../Curve.h"
#include <stdlib.h>

using namespace Curves;
using namespace Eigen;

struct SystemData {
	AffineCompact3f transform;
};

struct PhysicsVectors {
	Vector3f pos;
	Vector3f vel;
	Vector3f force;

	void updatePosAndVel(float mass) {
		pos = pos + vel;
		vel = vel + (force / mass);
		force = Vector3f();
	}

	void addForce(Vector3f force) {
		this->force = this->force + force;
	}

	void addVelocity(Vector3f vel) {
		this->vel = this->vel + vel;
	}

	void assertMaxVelocity(Vector3f max) {
		assertMax(vel, max);
	}

	void assertMinVelocity(Vector3f min) {
		assertMin(vel,min);
	}

private:
	void assertMin(Vector3f& v, Vector3f min) {
		for (int pos = 0; pos < 3; ++pos) {
			if(v[pos] < min[pos]) v[pos] = min[pos];
		}
	}

	void assertMax(Vector3f& v, Vector3f max) {
		for (int pos = 0; pos < 3; ++pos) {
			if(v[pos] > max[pos]) v[pos] = max[pos];
		}
	}
};

struct lifetimeData {
	bool fadeBegan;
	float fadeOutBegin;
	float fadeOutTime;
	float lifetimeCurrent;
	float lifetime;
	float lifetimeStart;
	float fadeInTime;

	bool fadingIn() {
		return lifetime < fadeInTime;
	}

	bool fadingOut() {
		return fadeBegan && (lifetimeElapsed() - fadeOutBegin) < fadeOutTime;
	}

	inline bool alive() {
		return getRemainingLifetime() > 0;
	}

	inline float getInterpolationValueFadeIn() {
		return lifetimeElapsed() / fadeInTime;
	}

	inline float getInterpolationValueFadeOut() {
		return (lifetimeElapsed() - fadeOutBegin) / fadeOutTime;
	}

	float getRemainingLifetime() {
		if(fadingOut()) return fadeOutTime - (lifetimeElapsed() - fadeOutBegin);
		else return lifetime - lifetimeElapsed();
	}

	bool setElapsed(float elapsed) {
		if(elapsed < 0) return false;

		lifetimeCurrent = elapsed;
		if(lifetimeCurrent > fadeOutBegin) {
			this->fadeBegan = true;
		}
		return true;
	}

	inline bool addElappsed(float delta) {
		return setElapsed(lifetimeCurrent + delta);
	}

	inline float lifetimeElapsed() {
		return lifetimeCurrent - lifetimeStart;
	}
};

struct UpdateData {
	Curve<float,float>* movementCurve;
	PhysicsVectors constantMoveVectors;

	void freeData() {
		if(movementCurve != NULL) delete movementCurve;
	}
};

struct RenderData {
	unsigned char alpha, minAlpha, maxAlpha;
	Vector3f size;
	//Color in RGB-space, given as float for graphics-cards.
	Vector3f color;

	RenderData() {
		memset(this, 0, sizeof(RenderData));
	}

	RenderData(unsigned int alpha, Vector3f size) {
		this->alpha = alpha;
		this->maxAlpha = alpha;
		this->minAlpha = alpha;
		this->size = size;
	}
};

struct StandardParticle {
	PhysicsVectors vec;
	Quaternionf rotation;
	lifetimeData lifetime;
	UpdateData update;
	RenderData render;
	bool boundToOrigin;

	StandardParticle() {
		memset(this, 0, sizeof(StandardParticle));
		update.movementCurve = NULL;
		boundToOrigin = false;
	}

	StandardParticle(Vector3f pos, Vector3f size, float lifetime) {
		memset(this, 0, sizeof(StandardParticle));
		update.movementCurve = NULL;
		this->lifetime.lifetime = lifetime;
		this->vec.pos = pos;
		this->render.size = size;
	}

	void freeParticle() {
		update.freeData();
	}

	inline bool isBoundToOrigin() {
		return boundToOrigin;
	}
};

#endif /* FIREPARTICLE_H_ */
