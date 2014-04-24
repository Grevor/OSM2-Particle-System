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
template<typename UpdateData>
class Updateable : Living {
public:
	/**
	 * Updates this object.
	 * @param data Data used to update object.
	 */
	virtual void update(UpdateData data) = 0;

	virtual ~Updateable() = 0;
};

#endif /* UPDATEABLE_H_ */
