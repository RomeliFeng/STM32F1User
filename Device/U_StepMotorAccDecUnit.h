/*
 * U_StepMotorAccDecUnit.h
 *
 *  Created on: 2017年11月7日
 *      Author: Romeli
 */

#ifndef SMSpeedCtlUnit_H_
#define SMSpeedCtlUnit_H_

#include <U_Debug.h>
#include <U_Misc.h>
#include "cmsis_device.h"
#include "U_StepMotor.h"

/*
 * author Romeli
 * ps 提取对象时线程不安全，确保提取操作只在主线程中进行
 */
class U_StepMotorAccDecUnit {
public:
	enum Mode_Typedef {
		Mode_Accel, Mode_Decel
	};

	//构造函数
	U_StepMotorAccDecUnit(TIM_TypeDef* TIMx);
	virtual ~U_StepMotorAccDecUnit();

	void Init();

	static void InitAll();

	static U_StepMotorAccDecUnit* GetFreeUnit(U_StepMotor* stepMotor);
	static void Free(U_StepMotor* stepMotor);
	void Free();
	void Lock(U_StepMotor* stepMotor);
	void Start(Mode_Typedef mode);
	void Stop();

	//内联函数
	inline bool IsBusy() {
		return _Busy;
	}
	inline bool IsDone() {
		return _Done;
	}
	inline void SetMode(Mode_Typedef mode) {
		_Mode = mode;
	}

	uint16_t GetCurSpeed();
	void SetCurSpeed(uint16_t speed);

	//中断服务函数
	void IRQ();
protected:
	TIM_TypeDef* _TIMx;	//速度计算用定时器
	U_StepMotor* _StepMotor;

	virtual void TIMRCCInit() = 0;
	virtual void ITInit() = 0;
private:
	static U_StepMotorAccDecUnit* _Pool[];
	static uint8_t _PoolSp;

	Mode_Typedef _Mode;
	uint16_t _MaxSpeed;
	uint32_t _Accel;
	uint32_t _Decel;
	bool _Done;
	volatile bool _Busy;

	void TIMInit();
};


#endif /* SMSpeedCtlUnit_H_ */
