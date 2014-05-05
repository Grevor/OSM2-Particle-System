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
#include <mutex>
#include <stddef.h>
#include <stdlib.h>
using namespace std;

#define PTR_ADD(p,a) (((char*)p) + (a))
#define PTR_SUB(p,a) (((char*)p) - (a))

template<typename Particle>
class NaiveParticlePool: public ParticlePool<Particle> {
	struct particleNode_t {
		struct particleNode_t* next;
		Particle p;
	};
	typedef struct particleNode_t *Node;

	volatile atomic<Node> freelist;
	volatile atomic<Node> living;
	volatile atomic<Node> updated;
	volatile atomic<Node> done;

	Node pool;
	int pSize;
	//boost::mutex livingIteratorCreationMutex;
	volatile atomic<int64_t> numLivingIterator;
	volatile atomic<int64_t> numLiving;
	pthread_mutex_t livingMutex;

public:
	NaiveParticlePool(int poolSize) {
		pool = (Node)calloc(poolSize, sizeof(struct particleNode_t));
		assert(pool != NULL);
		pSize = poolSize;
		freelist = NULL;
		updated = NULL;
		done = NULL;
		living = NULL;
		iteratorCreationHalted = false;
		//livingIteratorCreationMutex(); //Initialize mutex.
		numLivingIterator = 0;
		for(int i = 0; i < poolSize; i++){
			Node n = pool + i;
			n->next = freelist;
			freelist = n;
		}
		pthread_mutex_init(&livingMutex,NULL);
	}

	virtual ~NaiveParticlePool() {
		delete[] pool;
	}


	class StackIterator : public ParticleIterator<Particle> {
	protected:
		volatile atomic<Node>* stack;
		volatile atomic<Node>* doneStack;
		public:
			StackIterator(volatile atomic<Node>* stack, volatile atomic<Node>* returnStack) {
				this->stack = stack;
				this->doneStack = returnStack;
			}

			virtual bool hasNext() override {
				return *stack != NULL;
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

			Particle* operator ++() override {
				return this->next();
			}

			void stepComplete() override {

			}

			virtual ~StackIterator() {

			}
		};

		/**
		 * Class representing a living-particle-iterator. This iterator is slightly different from its peers
		 * in that it checks not only its own stack, but also if the update-stack is empty.
		 *
		 * Do note that this iterator may, in some cases, miss particles which are initialized the same frame as the
		 * iterator itself.
		 */
		class LivingIterator : public StackIterator {

			volatile atomic<Node>* updatePool;
			NaiveParticlePool* pool;
			volatile atomic<Node> currentNode, topNode, oldTop;
			//The lower half (32 bit) is the index of the current node, the upper is the old top of stack.
			//This will require a remake of all Node pointers to pool-indicies.
			volatile atomic<int64_t> currentNodeAndOldTop;
		public:
			LivingIterator(volatile atomic<Node>* stack, volatile atomic<Node>* donePool, volatile atomic<Node>* updatePool, NaiveParticlePool* pool) :
				StackIterator(stack, donePool) {
				this->updatePool = updatePool;
				this->pool = pool;
				topNode = *this->stack;
				currentNode = topNode;
				oldTop = NULL;
				this->pool->affectNumLivingIterators(1);
			}

			bool hasNext() override {
				return *updatePool != NULL || (currentNode != oldTop || topNode != *this->stack); //*StackIterator::stack != NULL;
			}

			Particle* next() override {
				while(true) {
					Particle* part = livingNext();//StackIterator::next();
					if(part == NULL) {
						if(!hasNext())
							return NULL;
					} else {
						return part;
					}
				}
				return NULL;
			}

			Particle* livingNext() {
				if(currentNode != oldTop) {
					Particle* p = &expected->p;
					currentNode = currentNode->next;
					return p;
				} else {
					//Set the old top to the "perceived top of stack", and sets a new "perceived top of stack"
					//The currentNode is set to this perceived top.
					oldTop = topNode;
					topNode = *this->stack;
					currentNode = topNode;
					return NULL;
				}
				return NULL;
			}

			Particle* livingNextM() {
				Node desired, expected;
				while(true) {
					expected = currentNode;
					if(expected != oldTop) {
						Particle* p = &expected->p;
						desired = expected->next;
						if(atomic_compare_exchange_strong(&this->currentNode, &expected, desired)) return p;
						else continue;//Redo if failed.
					} else {
						//Set the old top to the "perceived top of stack", and sets a new "perceived top of stack"
						//The currentNode is set to this perceived top.
						expected = topNode;
						desired = *this->stack;
						if(atomic_compare_exchange_strong(&topNode, &expected, desired)) {
							oldTop = expected;

						}

						oldTop = topNode;
						topNode = *this->stack;
						currentNode = topNode;
						return NULL;
					}
				}
				return NULL;
			}

			void done(Particle* p) override {

			}

			~LivingIterator() override {
				pool->affectNumLivingIterators(-1);
			}
		};

		friend StackIterator;
		friend LivingIterator;

	ParticleIterator<Particle>* getAllocationIterator() {
		return new StackIterator(&this->freelist, &this->updated);
	}

	ParticleIterator<Particle>* getUpdateIterator() {
		return new StackIterator(&this->living, &this->updated);
	}

	ParticleIterator<Particle>* getLivingIterator() {
		pthread_mutex_lock(&livingMutex);
		ParticleIterator<Particle>* iter = new LivingIterator(&this->updated, NULL, &this->living, this);
		pthread_mutex_unlock(&livingMutex);
		return iter;
	}

	void returnParticle(Particle* p) override {
		while(true) {
			Node expected = freelist;
			Node desired = getParticleNodePtr(p);
			desired->next = expected;
			if(std::atomic_compare_exchange_strong(&freelist,&expected,desired)) return;
		}
	}

	virtual void reset() {
		stepComplete();
		ParticleIterator<Particle>* iter = getUpdateIterator();
		while(iter->hasNext()) {
			this->returnParticle(iter->next());
		}
		delete iter;

		iter = getLivingIterator();
		while(iter->hasNext()) {
			this->returnParticle(iter->next());
		}
		delete iter;

		iter = new StackIterator(&this->done, NULL);
		while(iter->hasNext()) {
			this->returnParticle(iter->next());
		}
		delete iter;

		numLivingIterator = 0;
	}

	virtual void stepComplete() override {
		//haltIteratorCreation();
		pthread_mutex_lock(&livingMutex);
		waitForLivingIterators();
		//this->living = this->done;
		//append(&this->living, &this->updated);
		this->living = this->updated;
		this->updated = NULL;
		this->done = NULL;
		pthread_mutex_unlock(&livingMutex);
		//resumeIteratorCreation();
	}

	virtual int size() override {
		return pSize;
	}
private:
	bool iteratorCreationHalted;

	void haltIteratorCreation() {
		iteratorCreationHalted = true;
	}

	void resumeIteratorCreation() {
		iteratorCreationHalted = false;
	}

	void waitForLivingIterators() {
		while(numLivingIterator != 0) ;
	}

	void append(volatile atomic<Node>* dst, volatile atomic<Node>* src) {
		while(true) {
			Node expected = *src;
			if(expected == NULL) return;
			Node desired = expected->next;
			if(!atomic_compare_exchange_strong(src, &expected, desired)) continue;

			while(true) {
				Node dstExpected = *dst;
				expected->next = *dst;
				if(atomic_compare_exchange_strong(dst,&dstExpected,expected)) break;
			}
		}
	}


	static Node getParticleNodePtr(Particle* pa) {
#ifndef DEBUG
		return (Node)PTR_SUB(pa,offsetof(particleNode_t,p));
#else
		Node returnVal = (Node)PTR_SUB(pa,offsetof(particleNode_t,p));
		DEBUG_ASSERT(pool <= returnVal && returnVal < pool + pSize);
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
			//if(iteratorCreationHalted && delta > 0) continue;
			int64_t expected = numLivingIterator;
			int64_t desired = expected + delta;
			if(std::atomic_compare_exchange_strong(&this->numLivingIterator, &expected, desired)) return;
		}
	}




};

#endif /* NAIVEPARTICLEPOOL_H_ */
