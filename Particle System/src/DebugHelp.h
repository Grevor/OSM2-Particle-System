/*
 * DebugHelp.h
 *
 * A header simply for some define macros related to debugging.
 *
 *  Created on: 25 apr 2014
 *      Author: Erik
 */

#ifndef DEBUGHELP_H_
#define DEBUGHELP_H_

#ifdef DEBUG
#include <assert.h>
#define DEBUG_ASSERT(expr) assert(expr)
#else
#define DEBUG_ASSERT(expr)
#endif



#endif /* DEBUGHELP_H_ */
