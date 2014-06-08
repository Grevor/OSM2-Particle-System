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
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <atomic>
#include <stddef.h>
#include <stdlib.h>
using namespace std;

#define PTR_ADD(p,a) (((char*)p) + (a))
#define PTR_SUB(p,a) (((char*)p) - (a))

using boost::interprocess::interprocess_mutex;

/**
 * Class representing a particle pool with concurrent access.
 */
template<typename Particle>
class NaiveParticlePool: public ParticlePool<Particle> {
	/**
	 * Struct used by the inner stacks.
	 */
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
	interprocess_mutex* livingIteratorCreationMutex;

public:
	/**
	 * Creates a new NaiveParticlePool of the specified size.
	 * @param poolSize The size of the pool.
	 */
	NaiveParticlePool(int poolSize) {
		pool = (Node)calloc(poolSize, sizeof(struct particleNode_t));
		assert(pool != NULL);
		pSize = poolSize;
		freelist = NULL;
		updated = NULL;
		done = NULL;
		living = NULL;
		numLivingIterator = 0;
		for(int i = 0; i < poolSize; i++){
			Node n = pool + i;
			n->next = freelist;
			freelist = n;
		}
		livingIteratorCreationMutex = new interprocess_mutex();
	}

	virtual ~NaiveParticlePool() {
		delete[] pool;
	}


	/**
	 * Class
	 */
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
		 * Do note that this iterator may, in some cases, miss particles which are updated last in the previous iteration.
		 */
		class LivingIterator : public StackIterator {

			volatile atomic<Node>* updatePool;
			NaiveParticlePool* pool;
			volatile atomic<Node> currentNode, topNode, oldTop;
			//The lower half (32 bit) is the index of the current node, the upper is the old top of stack.
			//This will require a remake of all Node pointers to pool-indicies.
			volatile atomic<int64_t> currentNodeAndOldTop;
		public:
			/**
			 * Creates a new LivingIterator.
			 * @param stack The stack from which to get living elements.
			 * @param donePool The stack to put elements when they are done. [UNUSED]
			 * @param updatePool The stack which contains elements which may be of interest to the iterator.
			 * @param pool The NaiveParticlePool which this iterator belongs to.
			 */
			LivingIterator(volatile atomic<Node>* stack, volatile atomic<Node>* donePool, volatile atomic<Node>* updatePool, NaiveParticlePool* pool) :
				StackIterator(stack, donePool) {
				this->updatePool = updatePool;
				this->pool = pool;
				topNode = *this->stack;
				currentNode = topNode;
				oldTop = NULL;
				this->pool->affectNumLivingIterators(1);
			}

			/**
			 * Note that this function may miss up to n - 1 particles,
			 * where n is the number of threads accessing the iterator.
			 *
			 * This function may therefore yield both false negatives and false positives.
			 * @return True if this iterator has a next particle, else false.
			 */
			bool hasNext() override {
				return *updatePool != NULL || (currentNode != oldTop || topNode != *this->stack); //*StackIterator::stack != NULL;
			}

			Particle* next() override {
				while(true) {
					Particle* part = livingNext();
					if(part == NULL) {
						if(!hasNext())
							return NULL;
					} else {
						return part;
					}
				}
				return NULL;
			}

			/**
			 * [NOT THREAD-SAFE]
			 * Gets the next particle from this iterator.
			 * @return Pointer to the next particle. This Particle will not be given by this iterator again.
			 * If no particle could be found, return NULL.
			 */
			Particle* livingNext() {
				if(currentNode != oldTop) {
					Particle* p = &((Node)currentNode)->p;
					currentNode = ((Node)currentNode)->next;
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

			/**
			 * Gets the next living particle in a thread-safe manner.
			 * [NOT WORKING]
			 * @return
			 */
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

			/**
			 * Empty since we do not want anything to happen.
			 * @param p
			 */
			void done(Particle* p) override {

			}

			~LivingIterator() override {
				pool->affectNumLivingIterators(-1);
			}
		};//!LivingIterator

		friend StackIterator;
		friend LivingIterator;

	ParticleIterator<Particle>* getAllocationIterator() override {
		return new StackIterator(&this->freelist, &this->updated);
	}

	ParticleIterator<Particle>* getUpdateIterator() override {
		return new StackIterator(&this->living, &this->updated);
	}

	ParticleIterator<Particle>* getLivingIterator() override {
		livingIteratorCreationMutex->lock();
		ParticleIterator<Particle>* iter = new LivingIterator(&this->updated, NULL, &this->living, this);
		livingIteratorCreationMutex->unlock();
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

	/**
	 * [NOTE]
	 * This function is currently done in a naive and bad way.
	 */
	virtual void reset() override {
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
		livingIteratorCreationMutex->lock();
		waitForLivingIterators();
		this->living = this->updated;
		this->updated = NULL;
		this->done = NULL;
		livingIteratorCreationMutex->unlock();
	}

	virtual int size() override {
		return pSize;
	}
private:

	void waitForLivingIterators() {
		while(numLivingIterator != 0) ;
	}

	/**
	 * [UNUSED]
	 * Pushes a stack of nodes onto another stack of nodes.
	 * @param dst The stack to push to.
	 * @param src The stack to pop from.
	 */
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

	/**
	 * Gets the pointer to the stack node of a particle-pointer.
	 * @param pa The pointer to the particle for which to get the stack node pointer to.
	 * @return
	 * Pointer to the stack node.
	 */
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
