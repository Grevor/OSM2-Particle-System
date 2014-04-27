/*
 * demo.cpp
 *
 *  Created on: 27 apr 2014
 *      Author: Erik
 */


#include <stdio.h>
#include "ConstantCurve.h"
using namespace Curve;

int main(int argc, char **argv) {
	printf("Hello world!\n");
	ConstantCurve c(30);
	printf("Managed to create curve on stack.\n");
	return 0;
}
