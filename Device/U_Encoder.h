/*
 * U_Encoder.h
 *
 *  Created on: 2017年11月1日
 *      Author: Romeli
 */

#ifndef U_ENCODER_H_
#define U_ENCODER_H_

#include <U_Debug.h>
#include <U_Misc.h>
#include "cmsis_device.h"

class U_Encoder {
public:
	U_Encoder(TIM_TypeDef* TIMx, U_IT_Typedef& it);
	virtual ~U_Encoder();

	//初始化硬件
	static void InitAll();
	void Init();
	//设置当前位置
	void Set(int32_t pos);
	//获取当前位置
	int32_t Get() const;

	//中断服务子函数
	void IRQ();
protected:
	//编码器定时器
	TIM_TypeDef* _TIMx;

	virtual void GPIOInit() = 0;
	virtual void TIMRCCInit() = 0;
private:
	static U_Encoder* _Pool[];
	static uint8_t _PoolSp;

	U_IT_Typedef _IT; //中断优先级
	volatile int16_t _ExCNT;

	void TIMInit();
	void ITInit();
};

#endif /* U_ENCODER_H_ */
