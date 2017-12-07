/*
 * Debug.h
 *
 *  Created on: 2017年11月8日
 *      Author: Romeli
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "cmsis_device.h"
#include "diag/Trace.h"

namespace User {

#define DebugOut(message) (Debug::Print((uint8_t*) (__FILE__), __LINE__, message))

class Debug {
public:
	static void Print(uint8_t* file, uint32_t line, const char* message);
};

} /* namespace User */

#endif /* DEBUG_H_ */
