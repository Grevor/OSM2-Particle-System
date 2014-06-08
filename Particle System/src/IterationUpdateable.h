#ifndef UPDATEABLE_H_
#define UPDATEABLE_H_
#include "Living.h"
#include "Stepable.h"
/**
 * Interface describing an object which is updated over iterations.
 */
class IterationUpdateable : public Living, public Stepable {
public:
	/**
	 * Updates this object. Multiple threads should be able to call this method
	 * asynchronously to perform the update concurrently.
	 */
	virtual void update() = 0;

	virtual ~IterationUpdateable() {}
};

#endif /* UPDATEABLE_H_ */
