/*
 * SpawnCurves.hpp
 *
 *  Created on: 27 maj 2014
 */

#ifndef SPAWNCURVES_HPP_
#define SPAWNCURVES_HPP_
#include <curves/Curve.h>

class AllAtOnceSpawnCurve : public Curves::Curve<long,long> {
	long p;
public:
	AllAtOnceSpawnCurve(long particles) {
		p = particles;
	}

	long getValue(long currentIter) override {
		if(currentIter <= 1) return p;
		else return 0;
	}
};



#endif /* SPAWNCURVES_HPP_ */
