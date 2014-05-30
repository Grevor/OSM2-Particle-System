#ifndef CONCURVE_HPP_
#define CONCURVE_HPP_

#include "../../Particle System/src/Curve.h"

class IncCurve : public Curves::Curve<long,long> {
	long value;

public:
	IncCurve() {
		value = 0;
	}

	void spawn(int amount) {
        value += amount;
	}

	long getValue(long val) override {
		return value;
	}
};



#endif
