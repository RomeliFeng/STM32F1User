/*
 * U_StepMotorAccDecUnit.cpp
 *
 *  Created on: 2017年11月7日
 *      Author: Romeli
 */

#include <Device/U_StepMotorAccDecUnit.h>

U_StepMotorAccDecUnit* U_StepMotorAccDecUnit::_Pool[4];
uint8_t U_StepMotorAccDecUnit::_PoolSp = 0;

/*
 * author Romeli
 * explain 把自身加入资源池，并且初始化变量
 * return com
 */
U_StepMotorAccDecUnit::U_StepMotorAccDecUnit(TIM_TypeDef* TIMx) {
	_TIMx = TIMx;
	//自动将对象指针加入资源池
	_Pool[_PoolSp++] = this;

	/* Do not care what their value */
	_StepMotor = 0;
	_Mode = StepMotorAccDecUnitMode_Accel;
	_MaxSpeed = 10000;
	_Accel = 20000;
	_Decel = 20000;
	_Busy = false;
	_Done = false;
}

U_StepMotorAccDecUnit::~U_StepMotorAccDecUnit() {
	// TODO Auto-generated destructor stub
}

/*
 * author Romeli
 * explain 初始化速度计算单元（此函数应在派生类的对象中重写）
 * return void
 */
void U_StepMotorAccDecUnit::Init() {
	_StepMotor = 0;
	_Busy = false;
	_Done = false;
	TIMInit();
	ITInit();
}

/*
 * author Romeli
 * explain 初始化所有速度计算单元
 * return void
 */
void U_StepMotorAccDecUnit::InitAll() {
	//初始化池内所有单元
	for (uint8_t i = 0; i < _PoolSp; ++i) {
		_Pool[i]->Init();
	}
	if (_PoolSp == 0) {
		//Error @Romeli 无速度计算单元（无法进行运动）
		U_DebugOut("There have no speed control unit exsit");
	}
}

/*
 * author Romeli
 * explain 从速度计算单元池中提取一个可用单元
 * return SMSCUnit* 可用单元的指针
 */
U_StepMotorAccDecUnit* U_StepMotorAccDecUnit::GetFreeUnit(
		U_StepMotor* stepMotor) {
	//遍历池内所有单元
	U_StepMotorAccDecUnit* unit;
	for (uint8_t i = 0; i < _PoolSp; ++i) {
		unit = _Pool[i];
		if (!unit->_Busy) {
			//如果当前单元空闲，锁定当前单元供本次运动使用
			unit->Lock(stepMotor);
			return unit;
		} else {
			if (!unit->_StepMotor->_Busy) {
				//Error @Romeli 释放了一个被锁定的速度控制单元（待验证）
				U_DebugOut("There have no speed control unit exsit");
				//如果当前单元被占用，但是运动模块空闲，视为当前单元空闲，锁定当前单元供本次运动使用
				unit->Free();
				unit->Lock(stepMotor);
				return unit;
			}
		}
	}
	//Error @Romeli 无可用的速度计算单元（超出最大同时运动轴数，应该避免）
	U_DebugOut("There have no available speed control unit");
	return 0;
}

/*
 * author Romeli
 * explain 释放当前速度计算单元
 * return void
 */
void U_StepMotorAccDecUnit::Free(U_StepMotor* stepMotor) {
	U_StepMotorAccDecUnit* unit;
	//释放当前运动模块所占用的加减速单元
	for (uint8_t i = 0; i < _PoolSp; ++i) {
		unit = _Pool[i];
		if (unit->_StepMotor == stepMotor) {
			unit->Free();
		}
	}
}

/*
 * author Romeli
 * explain 解锁当前单元
 * return void
 */
void U_StepMotorAccDecUnit::Free() {
	//关闭当前单元
	Stop();
	//复位标志位，解锁当前单元
	_Busy = false;
}

/*
 * author Romeli
 * explain 锁定当前单元准备运动
 * param stepMotor 欲使用当前单元的运动模块
 * return void
 */
void U_StepMotorAccDecUnit::Lock(U_StepMotor* stepMotor) {
	//存储当前单元的运动模块
	_StepMotor = stepMotor;
	//存储最大速度
	_MaxSpeed = _StepMotor->_MaxSpeed;
	//存储加速度
	_Accel = _StepMotor->_Accel;
	//存储减速度
	_Decel = _StepMotor->_Decel;
	//置忙标志位，锁定当前单元
	_Busy = true;
}

/*
 * author Romeli
 * explain 启动速度计算单元
 * param1 dir 加速还是减速
 * param2 tgtSpeed 目标速度
 * return void
 */
void U_StepMotorAccDecUnit::Start(StepMotorAccDecUnitMode_Typedef mode) {
	//关闭可能存在的计算任务
	Stop();
	SetMode(mode);
	_Done = false;

	uint16_t initSpeed = STEP_MOTOR_MIN_SPEED;
	switch (_Mode) {
	case StepMotorAccDecUnitMode_Accel:
		_TIMx->PSC = (uint16_t) (SystemCoreClock / _Accel);
		_TIMx->ARR = _MaxSpeed;
		initSpeed = STEP_MOTOR_MIN_SPEED;
		break;
	case StepMotorAccDecUnitMode_Decel: {
		uint16_t speed = _TIMx->CNT;
		_TIMx->PSC = (uint16_t) (SystemCoreClock / _Decel);
		_TIMx->ARR = (uint16_t) (_MaxSpeed - STEP_MOTOR_MIN_SPEED);
		initSpeed = (uint16_t) (_MaxSpeed - speed);
		break;
	}
	default:
		break;
	}
	TIM_PSC_RELOAD(_TIMx);	//更新时会清空CNT，需要注意
	_TIMx->CNT = initSpeed;

	//开始速度计算
	TIM_CLEAR_UPDATE_FLAG(_TIMx);
	TIM_ENABLE_IT_UPDATE(_TIMx);
	TIM_ENABLE(_TIMx);
}

/*
 * author Romeli
 * explain 关闭当前速度计算单元
 * return void
 */
void U_StepMotorAccDecUnit::Stop() {
	//关闭速度计算定时器
	TIM_DISABLE_IT_UPDATE(_TIMx);
	TIM_DISABLE(_TIMx);
	//清除速度计算定时器中断标志
}

/*
 * author Romeli
 * explain 根据TIM寄存器计算当前速度
 * return uint16_t
 */
uint16_t U_StepMotorAccDecUnit::GetCurSpeed() {
	//读取当前速度
	uint16_t speed = _Done ? _TIMx->ARR : _TIMx->CNT;
	switch (_Mode) {
	case StepMotorAccDecUnitMode_Accel:
		return speed;
		break;
	case StepMotorAccDecUnitMode_Decel:
		//计算当前速度（部分定时器没有向下计数）
		return (uint16_t) (_MaxSpeed - speed);
		break;
	default:
		//Error @Romeli 错误的状态，不应该发生（超出最大同时运动轴数，应该避免）
		U_DebugOut("Status Error!");
		return STEP_MOTOR_MIN_SPEED;
		break;
	}
}

/*
 * author Romeli
 * explain 设置当前速度，用于加速度为0的情况
 * param speed 速度
 * return void
 */
void U_StepMotorAccDecUnit::SetCurSpeed(uint16_t speed) {
	if (speed < 200) {
		//Error @Romeli 速度小于最低速度
		U_DebugOut("There have no available speed control unit");
		speed = 200;
	}
	_TIMx->CNT = speed;
}

/*
 * author Romeli
 * explain 速度计算单元更新速度用的中断服务子程序
 * return void
 */
void U_StepMotorAccDecUnit::IRQ() {
	_Done = true;
	_TIMx->CNT = _TIMx->ARR;
	//停止
	Stop();
}

/*
 * author Romeli
 * explain 初始化中断设置（此函数应在派生类中重写）
 * return void
 */
void U_StepMotorAccDecUnit::ITInit() {
	U_DebugOut("This function should be override");
	/*	NVIC_InitTypeDef NVIC_InitStructure;
	 //设置中断
	 NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
	 SMSpeedCtlUnit1_TIM7_IRQn.ITPriority_PreemptionPriority;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority =
	 SMSpeedCtlUnit1_TIM7_IRQn.ITPriority_SubPriority;
	 NVIC_Init(&NVIC_InitStructure);*/
}

/*
 * author Romeli
 * explain 初始化定时器设置（此函数应在派生类中重写）
 * return void
 */
void U_StepMotorAccDecUnit::TIMInit() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIMRCCInit();

	TIM_DeInit(_TIMx);
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0xffff;
	TIM_TimeBaseInitStructure.TIM_Period = 0xffff;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(_TIMx, &TIM_TimeBaseInitStructure);

	TIM_ARRPreloadConfig(_TIMx, ENABLE);
}
