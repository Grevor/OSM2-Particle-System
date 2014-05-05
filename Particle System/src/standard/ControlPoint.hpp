/*
 * ControlPoint.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef CONTROLPOINT_HPP_
#define CONTROLPOINT_HPP_
#include <Eigen/Eigen>

using namespace Eigen;

class ControlPoint {
public:
	virtual Quaternion rotation() = 0;

	virtual Vector3f position() = 0;

	virtual ~ControlPoint() {}
};


#endif /* CONTROLPOINT_HPP_ */
