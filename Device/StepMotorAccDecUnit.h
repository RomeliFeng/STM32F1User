/*
 * StepMotorAccDecUnit.h
 *
 *  Created on: 2017年11月7日
 *      Author: Romeli
 */

#ifndef SMSpeedCtlUnit_H_
#define SMSpeedCtlUnit_H_

#include "cmsis_device.h"
#include "ITPriority.h"
#include "Debug.h"
#include "Typedef.h"
#include "StepMotor.h"

namespace User {
namespace Device {

typedef enum {
	StepMotorAccDecUnitMode_Accel, StepMotorAccDecUnitMode_Decel
} StepMotorAccDecUnitMode_Typedef;

/*
 * author Romeli
 * ps 提取对象时线程不安全，确保提取操作只在主线程中进行
 */
class StepMotorAccDecUnit {
public:
	//构造函数
	StepMotorAccDecUnit();
	virtual ~StepMotorAccDecUnit();

	static void InitAll();
	virtual void Init();

	static StepMotorAccDecUnit* GetFreeUnit(StepMotor* stepMotor);
	static void Free(StepMotor* stepMotor);
	void Free();
	void Lock(StepMotor* stepMotor);
	void Start(StepMotorAccDecUnitMode_Typedef mode);
	void Stop();

	//内联函数
	inline bool IsBusy() {
		return _Busy;
	}
	inline bool IsDone() {
		return _Done;
	}
	inline void SetMode(StepMotorAccDecUnitMode_Typedef mode) {
		_Mode = mode;
	}

	uint16_t GetCurSpeed();
	void SetCurSpeed(uint16_t speed);

	//中断服务函数
	void SMSpeedCtlIRQ();
protected:
	TIM_TypeDef* _TIMx;	//速度计算用定时器
	StepMotor* _StepMotor;

	virtual void TIMInit();
	virtual void ITInit();
private:
	static StepMotorAccDecUnit* _Pool[];
	static uint8_t _PoolSp;

	StepMotorAccDecUnitMode_Typedef _Mode;
	uint16_t _MaxSpeed;
	uint32_t _Accel;
	uint32_t _Decel;
	bool _Done;
	volatile bool _Busy;
};

} /* namespace Device */
} /* namespace User */

#endif /* SMSpeedCtlUnit_H_ */
