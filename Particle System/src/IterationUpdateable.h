/*
 * Updateable.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef UPDATEABLE_H_
#define UPDATEABLE_H_
#include "Living.h"
/**
 * Interface describing an object which can be updated.
 */
class IterationUpdateable : Living, Stepable {
public:
	/**
	 * Updates this object.
	 * @param data Data used to update object.
	 */
	virtual void update() = 0;

	virtual ~IterationUpdateable() {}
};

#endif /* UPDATEABLE_H_ */
