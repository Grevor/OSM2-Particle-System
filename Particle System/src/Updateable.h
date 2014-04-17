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
public:
	virtual void update(T*) = 0;

	virtual ~Updateable() = 0;
};

#endif /* UPDATEABLE_H_ */
