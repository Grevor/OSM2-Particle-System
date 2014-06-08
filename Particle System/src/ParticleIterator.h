#ifndef PARTICLEITERATOR_H_
#define PARTICLEITERATOR_H_

/**
 * A ParticleIterator iterates over pointers to particles (an arbitrary type) in some collection of particles.
 * This interface does not guarantee thread safety, but specific implementation may do.
 */
template<typename T>
class ParticleIterator {
public:
	/**
	 * Fetches the first element in the iterator and steps it forward.
	 * @return first element of this iterator if it exists, else NULL.
	 */
	virtual T* next() = 0;
	/**
	 * Checks if this iterator has more elements.
	 * @return true if this iterator has 1 or more elements left.
	 */
	virtual bool hasNext() = 0;
	/**
	 * @see next()
	 * @return
	 */
	virtual T* operator++() = 0;

	/**
	 * Tells the iterator that the last time-step was completed,
	 * and a new one is in-bound.
	 */
	virtual void stepComplete() = 0;

	/**
	 * Tell the iterator that the user is done with the particle.
	 * @param particle the particle.
	 */
	virtual void done(T* particle) = 0;

	virtual ~ParticleIterator() {}
};

#endif /* PARTICLEITERATOR_H_ */
