/*
 * Updateable.h
 *
 *  Created on: 16 apr 2014
 *      Author: David
 */

#ifndef UPDATEABLE_H_
#define UPDATEABLE_H_

template<class T>
class Updateable : Living {
	virtual void update(T);
};

#endif /* UPDATEABLE_H_ */
