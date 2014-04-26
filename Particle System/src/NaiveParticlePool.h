/*
 * NaiveParticlePool.h
 *
 *  Created on: 25 apr 2014
 *      Author: Erik
 */

#ifndef NAIVEPARTICLEPOOL_H_
#define NAIVEPARTICLEPOOL_H_

#include "ParticlePool.h"
#include <boost/atomic/atomic.hpp>
#include <atomic>
#include <stddef.h>
#include <stdlib.h>

#define PTR_ADD(p,a) (((char*)p) + (a))
#define PTR_SUB(p,a) (((char*)p) - (a))

template<typename Particle>
class NaiveParticlePool: public ParticlePool<Particle> {
	struct particleNode_t {
		struct particleNode_t* next;
		Particle p;
	};
	typedef struct particleNode_t *Node;

public:
	NaiveParticlePool(int poolSize) {
		pool = calloc(poolSize, sizeof(struct particleNode_t));
		assert(pool != NULL);
		size = poolSize;
		freelist = NULL;
		updated = NULL;
		done = NULL;
		living = NULL;
		livingIteratorCreationMutex(); //Initialize mutex.
		numLivingIterator = 0;
		for(int i = 0; i < poolSize; i++){
			Node n = pool + i;
			n->next = freelist;
			freelist = n;
		}
	}
	virtual ~NaiveParticlePool() {
		delete[] pool;
	}



	ParticleIterator<Particle>* getAllocationIterator() {
		return new StackIterator(&this->freelist, &this->living);
	}

	ParticleIterator<Particle>* getUpdateIterator() {
		return new StackIterator(&this->living, &this->update);
	}

	ParticleIterator<Particle>* getLivingIterator() {
		this->affectNumLivingIterators(1);
		return new LivingIterator(&this->updated, &this->done, &this->updated, this);
	}

	void returnParticle(Particle* p) {
		while(true) {
			Node expected = freelist;
			Node desired = getParticleNodePtr(p);
			if(std::atomic_compare_exchange_strong(&freelist,&expected,desired)) return;
		}
	}

	virtual void reset() {
		stepComplete();
		ParticleIterator<Particle>* iter = getUpdateIterator();
		while(iter->hasNext()) {
			this->returnParticle(iter->next());
		}

		iter = getLivingIterator();
		while(iter->hasNext()) {
			this->returnParticle(iter->next());
		}

		iter = new StackIterator(&this->done, NULL);
		while(iter->hasNext()) {
			this->returnParticle(iter->next());
		}

		numLivingIterator = 0;
	}

	virtual void stepComplete() override {
		waitForLivingIterators();
		this->living = this->done;
		this->done = NULL;
	}

private:

	void waitForLivingIterators() {
		while(numLivingIterator != 0) ;
	}


	static Node* getParticleNodePtr(Particle* pa) {
#ifndef DEBUG
		return PTR_SUB(p,offsetof(particleNode_t,p));
#else
		Node* returnVal = PTR_SUB(p,offsetof(particleNode_t,p));
		DEBUG_ASSERT(pool <= returnVal && returnVal < pool + size);
		DEBUG_ASSERT(PTR_SUB(returnVal, (size_t)pool) % sizeof(struct particleNode_t) == 0);
		return returnVal;
#endif
	}

	/**
	 * Changes the number of living iterators bound to the pool.
	 * This function asserts atomic operations.
	 * @param delta The change to invoke.
	 */
	void affectNumLivingIterators(int delta) {
		//TODO release a living iterator.
		while(true) {
		size_t expected = numLivingIterator;
		size_t desired = expected - delta;
		if(std::atomic_compare_exchange_strong(&this->numLivingIterator, &expected, desired)) return;
		}
	}

	class StackIterator : ParticleIterator<Particle> {
	protected:
		Node* stack;
		Node* doneStack;
	public:
		StackIterator(Node* stack, Node* returnStack) {
			this->stack = stack;
			this->doneStack = returnStack;
		}

		virtual bool hasNext() override {
			return *stack == NULL;
		}

		virtual Particle* next() override {
			while(true) {
				Node expected = *stack;
				if(expected == NULL) return NULL;
				Node desired = expected->next;
				if(std::atomic_compare_exchange_strong(stack,&expected,desired)) return &expected->p;
			}
			return NULL;
		}

		virtual void done(Particle* p) override {
			while(true) {
				Node expected = *doneStack;
				Node desired = getParticleNodePtr(p);
				desired->next = expected;
				if(std::atomic_compare_exchange_strong(doneStack,&expected,desired)) return;
			}
		}

		StackIterator operator ++() override {
			return this->next();
		}

		virtual ~StackIterator() {

		}
	};

	/**
	 * Class representing a living-particle-iterator. This iterator is slightly different from its peers
	 * in that it checks not only its own stack, but also if the update-stack is empty.
	 *
	 * Do note that this iterator may, in some cases, miss particles which are initialized the same frame as the
	 */
	class LivingIterator : StackIterator {

		Node* updatePool;
		NaiveParticlePool* pool;
		LivingIterator(Node* stack, Node* donePool, Node* updatePool, NaiveParticlePool* pool) :
			StackIterator(stack, donePool) {
			this->updatePool = updatePool;
			this->pool = pool;
		}

		bool hasNext() override {
			return *stack != NULL && *updatePool != NULL;
		}

		Particle* next() override {
			while(true) {
				Particle* part = StackIterator::next();
				if(part == NULL) {
					if(!hasNext()) return NULL;
					else continue;
				} else {
					return part;
				}
			}
			return NULL;
		}

		~LivingIterator() {
			pool->affectNumLivingIterators(-1);
		}
	};

	volatile Node freelist;
	volatile Node living;
	volatile Node updated;
	volatile Node done;

	Node pool;
	int size;
	boost::mutex livingIteratorCreationMutex;
	volatile size_t numLivingIterator;
};

#endif /* NAIVEPARTICLEPOOL_H_ */
