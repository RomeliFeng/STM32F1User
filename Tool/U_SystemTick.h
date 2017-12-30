/*
 * U_SystemTick.h
 *
 *  Created on: 20171213
 *      Author: Romeli
 */

#ifndef U_SYSTEMTICK_H_
#define U_SYSTEMTICK_H_

#include "cmsis_device.h"
#include "U_Debug.h"

class U_SystemTick {
public:
	static void Init(uint16_t us);
	static void WaitMicroSecond(uint64_t us);
	static inline void WaitMilliSecond(uint32_t ms) {
		WaitMicroSecond(ms * 1000);
	}
	static inline void WaitSecond(uint32_t s) {
		WaitMilliSecond(s * 1000);
	}
	static uint64_t GetMilliSecond();
	static uint64_t GetMicroSecond();
	static void IRQ();
private:
	static bool _InitFlag;
	static volatile uint_fast64_t _Now;
	static uint_fast64_t _Last;
	static uint16_t _Interval;
};

#endif /* U_SYSTEMTICK_H_ */
