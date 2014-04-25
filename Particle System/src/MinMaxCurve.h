/*
 * MinMaxCurve.h
 *
 *  Created on: 25 apr 2014
 *      Author: Elin
 */

#ifndef MINMAXCURVE_H_
#define MINMAXCURVE_H_
#include "Curve.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

namespace Curve {
class MinMaxCurve : Curve<long, long>{
private:
	Curve<long,long> *Curve1;
	Curve<long, long> *Curve2;
	//long min;
	//long max;
public:
	~MinMaxCurve(){}
	long getValue(long val){
		long temp;
		long var = this->Curve1->getValue(val);
		long var2 = this->Curve2->getValue(val);
		if (var > var2)  temp = getMinMax(var2, var);
		else if (var2 > var) temp = getMinMax(var, var2);
		else temp = var;
		return temp;
	}

	MinMaxCurve(Curve<long,long> *one, Curve<long, long> *two){
		this->Curve1 = one;
		this->Curve2 = two;
	}

	long getMinMax (long mini, long maxi){
		return rand % (maxi-mini) + 1 + mini;
	}
};

}




#endif /* MINMAXCURVE_H_ */
