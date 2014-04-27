/*
 * ConstantCurve.h
 *
 *  Created on: 25 apr 2014
 *      Author: Elin
 */
#ifndef CONSTANTCURVE_H_
#define CONSTANTCURVE_H_

#include "Curve.h"

namespace Curve {
class ConstantCurve : Curve<long,long>
{
private:
	long value;
public:
	~ConstantCurve(){}
	long getValue(long val)override{
		return this->value;
	}

	ConstantCurve(long val) : Curve<long,long>(){
		value = val;
	}
};


}

#endif /* CONSTANTCURVE_H_ */
