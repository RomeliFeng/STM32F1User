/*
 * Debug.cpp
 *
 *  Created on: 2017年11月8日
 *      Author: Romeli
 */

#include <Debug.h>

namespace User {

void Debug::Print(uint8_t* file, uint32_t line, const char* message) {
#ifdef USER_DEBUG
	trace_printf("U_assert() failed: file \"%s\", line %d, message \"%s\"",
			file, line, message);
	trace_printf("\n");
#endif
}

} /* namespace User */

