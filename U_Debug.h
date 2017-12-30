/*
 * U_Debug.h
 *
 *  Created on: 2017年11月8日
 *      Author: Romeli
 */

#ifndef U_DEBUG_H_
#define U_DEBUG_H_

#include "cmsis_device.h"
#include "diag/Trace.h"

#define U_DebugOut(message) (U_Debug::Print((uint8_t*) (__FILE__), __LINE__, message))

class U_Debug {
public:
	static void Print(uint8_t* file, uint32_t line, const char* message);
};


#endif /* U_DEBUG_H_ */
