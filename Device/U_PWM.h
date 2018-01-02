/*
 * U_PWM.h
 *
 *  Created on: 2018年1月2日
 *      Author: Romeli
 */

#ifndef DEVICE_U_PWM_H_
#define DEVICE_U_PWM_H_

#include "cmsis_device.h"

class U_PWM {
public:
	U_PWM(TIM_TypeDef* TIMx);
	virtual ~U_PWM();

	//初始化硬件
	static void InitAll();
	void Init();
private:
	static U_PWM* _Pool[4];
	static uint8_t _PoolS;
};

#endif /* DEVICE_U_PWM_H_ */
