/*
 * Updateable.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef UPDATEABLE_H_
#define UPDATEABLE_H_

/**
 * Interface describing an object which can be updated.
 */
class IterationUpdateable : Living {
public:

	/**
	 * Steps this object, making it progress to its next iteration if all
	 * updating from the previous frame has been completed.
	 *
	 * If the update is not complete, an error may be thrown.
	 */
	virtual void step() = 0;

	/**
	 * Updates this object.
	 * @param data Data used to update object.
	 */
	virtual void update() = 0;

	virtual ~IterationUpdateable() = 0;
};

#endif /* UPDATEABLE_H_ */
