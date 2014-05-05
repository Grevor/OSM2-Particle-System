/*
 * Stepable.h
 *
 *  Created on: 28 apr 2014
 *      Author: David
 */

#ifndef STEPABLE_H_
#define STEPABLE_H_

/**
 * A Stepable object is something which is updated in several iterations, called steps.
 */
class Stepable {
public:
	virtual ~Stepable();
	/**
	 * Steps this Stepable forward. If the Stepable isn't ready to step forward yet, it should block
	 * until it's ready, and then step.
	 */
	virtual void step() = 0;
	/**
	 * Checks if all updates for this Stepable has been completed this step.
	 * If the update have been completed, the Stepable is ready to move on to the next step.
	 * @return true if updates this step is completed, else false.
	 */
	virtual bool isStepComplete() = 0;
};

#endif /* STEPABLE_H_ */
