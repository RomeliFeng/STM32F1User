/*
 * U_SystemTick.cpp
 *
 *  Created on: 20171213
 *      Author: Romeli
 */

#include <Tool/U_SystemTick.h>

bool U_SystemTick::_InitFlag = false;
volatile uint_fast64_t U_SystemTick::_Now = 0;
uint_fast64_t U_SystemTick::_Last = 0;
uint16_t U_SystemTick::_Interval = 0;

/*
 * author Romeli
 * explain 初始化系统滴答计时
 * param us 最小时间刻度，单位微妙
 * return void
 */
void U_SystemTick::Init(uint16_t us) {
	_Interval = us;
	SysTick_Config((SystemCoreClock / (1000000 / _Interval)) - 5); //Set SysTick timer=us
	NVIC_SetPriority(SysTick_IRQn, 0);					//Set SysTick interrupt
	_InitFlag = true;
}

/*
 * author Romeli
 * explain 等待一段时间
 * param us 等待的时间，微妙
 * return void
 */
void U_SystemTick::WaitMicroSecond(uint64_t us) {
	if (_InitFlag) {
		_Last = _Now;					//Record time_now
		while ((_Now - _Last) < us)
			;
	} else {
		//Error @Romeli 系统滴答没有初始化
		U_DebugOut("System tick has no be inited");
	}
}

/*
 * author Romeli
 * explain 获取开机到现在的计时，单位微妙
 * return uint64_t
 */
uint64_t U_SystemTick::GetMilliSecond() {
	return _Now / 1000;
}

/*
 * author Romeli
 * explain 获取开机到现在的计时，单位毫秒
 * return uint64_t
 */
uint64_t U_SystemTick::GetMicroSecond() {
	return _Now;
}

/*
 * author Romeli
 * explain 中断服务子程序，用于计时
 */
void U_SystemTick::IRQ() {
	_Now += _Interval;
}

extern "C" void SysTick_Handler() {
	U_SystemTick::IRQ();
}
