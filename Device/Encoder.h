/*
 * Encoder.h
 *
 *  Created on: 2017年11月1日
 *      Author: Romeli
 */

#ifndef U_ENCODER_H_
#define U_ENCODER_H_

#include "cmsis_device.h"
#include "ITPriority.h"

namespace User {
namespace Device {

class Encoder {
public:
	Encoder();
	virtual ~Encoder();

	//初始化硬件
	void Init();
	//设置当前位置
	void Set(int32_t pos);
	//获取当前位置
	int32_t Get() const;

	//中断服务子函数
	void IRQAction();
protected:
	//编码器定时器
	TIM_TypeDef *_TIMx;

	virtual void GPIOInit();
	virtual void TIMInit();
	virtual void ITInit();
private:
	volatile int16_t _ExCNT;
};

} /* namespace Device */
} /* namespace User*/
#endif /* U_ENCODER_H_ */
