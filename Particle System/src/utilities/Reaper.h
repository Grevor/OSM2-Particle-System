#ifndef REAPER_H_
#define REAPER_H_

/**
 * A Reaper determines if particles are dead or not.
 */
template<typename T>
class Reaper {
public:
	Reaper() {}

	/**
	 * Checks if a certain particle is dead.
	 * @param particle The particle.
	 * @return true if it is dead, else false.
	 */
	virtual bool isDead(T* particle) = 0;

	virtual ~Reaper() {}
};

#endif /* REAPER_H_ */
