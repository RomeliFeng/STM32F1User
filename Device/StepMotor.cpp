/*
 * StepMotor.cpp
 *
 *  Created on: 2017年11月6日
 *      Author: Romeli
 */

#include <Device/StepMotorAccDecUnit.h>
#include "StepMotor.h"

namespace User {
namespace Device {

StepMotor* StepMotor::_Pool[4];
uint8_t StepMotor::_PoolSp = 0;

StepMotor::StepMotor() {
	//自动将对象指针加入资源池
	_Pool[_PoolSp++] = this;

	_TIMx = 0;
	_TIMx_CCRx = 0;
	_TIMy_FRQ = 0;

	_AccDecUnit = 0;	//速度计算单元

	_CurStep = 0;	//当前已移动步数
	_TgtStep = 0;	//目标步数
	_DecelStartStep = 0;	//减速开始步数

	_Accel = 20000;	//加速度
	_Decel = 20000;	//减速度
	_MaxSpeed = 10000;	//最大速度

	_CWLimit = 0; //正转保护限位
	_CCWLimit = 0; //反转保护限位

	_RelativeDir = StepMotorDir_CW; //实际方向对应
	_Dir = StepMotorDir_CW; //当前方向

	_Status = StepMotorStatus_Stop;
	_StepLimit = true;
	_Busy = false;	//当前电机繁忙?
}

StepMotor::~StepMotor() {
}

void StepMotor::InitAll() {
	//初始化池内所有运动模块
	for (uint8_t i = 0; i < _PoolSp; ++i) {
		_Pool[i]->Init();
	}
	if (_PoolSp == 0) {
		//Error @Romeli 无运动模块（无法进行运动）
		Debug::Print((uint8_t*) __FILE__, __LINE__,
				"There have no speed control unit exsit");
	}
	//初始化所有的速度计算单元
	StepMotorAccDecUnit::InitAll();
}

/*
 * author Romeli
 * explain
 * param
 * return void
 */
void StepMotor::Init() {
	_TIMx = TIM8;
	_TIMx_CCRx = &TIM8->CCR2;
	GPIOInit();
	TIMInit();
	ITInit();
	_TIMy_FRQ = SystemCoreClock / (TIM8->PSC + 1);
}

/*
 * author Romeli
 * explain 移动步进电机
 * param1 step 欲移动的步数（为0时不会主动停止）
 * param2 dir 运动方向
 * return void
 */
Status_Typedef StepMotor::Move(uint32_t step, StepMotorDir_Typedef dir) {
	//停止如果有的运动
	Stop();
	//锁定当前运动
	_Busy = true;
	//获取可用的速度计算单元
	_AccDecUnit = StepMotorAccDecUnit::GetFreeUnit(this);
	if (_AccDecUnit == 0) {
		//没有可用的速度计算单元，放弃本次运动任务
		Debug::Print((uint8_t*) __FILE__, __LINE__,
				"Get speed control unit fail,stop move");
		_Busy = false;
		return Status_Error;
	}
	//清零当前计数
	_CurStep = 0;
	if (step != 0) {
		//基于步数运动
		_StepLimit = true;
		_TgtStep = step;

		if (_Decel != 0) {
			//当减速存在并且处于步数控制的运动流程中
			//从最高速减速
			uint32_t tmpStep = GetDecelStep(_MaxSpeed);
			//无法到达最高速度
			uint32_t tmpStep2 = _TgtStep >> 1;
			//当无法到达最高速度时，半步开始减速
			_DecelStartStep = (
					tmpStep >= tmpStep2 ? tmpStep2 : _TgtStep - tmpStep) - 1;
		} else {
			//减速度为0
			_DecelStartStep = 0;
		}
	} else {
		//持续运动，
		_StepLimit = false;
		_TgtStep = 0;
		_DecelStartStep = 0;
	}

	//设置方向
	SetDir(dir);

	//切换步进电机状态为加速
	_Status = StepMotorStatus_Accel;
	_AccDecUnit->SetMode(StepMotorAccDecUnitMode_Accel);

	if (_Accel != 0) {
		//开始加速 目标速度为最大速度
		_AccDecUnit->Start(StepMotorAccDecUnitMode_Accel);
	} else {
		_AccDecUnit->SetCurSpeed(_MaxSpeed);
	}
	//Warnning 需确保当前流程为加速
	SetSpeed(_AccDecUnit->GetCurSpeed());

	TIM_PSC_RELOAD(_TIMx);
	//开始输出脉冲
	TIM_CLEAR_UPDATE_FLAG(_TIMx);
	TIM_ENABLE_IT(_TIMx);
	TIM_ENABLE(_TIMx);
	return Status_Ok;
}

/*
 * author Romeli
 * explain 立即停止步进电机
 * return void
 */
void StepMotor::Stop() {
	//关闭脉冲发生
	TIM_DISABLE(_TIMx);
	TIM_DISABLE_IT(_TIMx);
	//尝试释放当前运动模块占用的单元
	StepMotorAccDecUnit::Free(this);
	//清空忙标志
	_Busy = false;
}

/*
 * author Romeli
 * explain 缓慢步进电机
 * return void
 */
void StepMotor::StopSlow() {
	//根据当前步数和从当前速度减速所需步数计算目标步数
	_TgtStep = GetDecelStep(_MaxSpeed) + _CurStep;
	//变更模式为步数限制
	_StepLimit = true;
	StartDec();
}

/*
 * author Romeli
 * explain 脉冲发生计数用中断服务子函数
 * return void
 */
void StepMotor::PulIRQ() {
	_CurStep++;
	//处于步数限制运动中 并且 到达指定步数，停止运动

	switch (_Status) {
	case StepMotorStatus_Accel:
		//_DecelStartStep = 0 时为持续运动模式
		if (_DecelStartStep != 0) {
			//当减速流程存在
			if (_CurStep >= _DecelStartStep) {
				//到达减速步数，进入减速流程
				StartDec();
				_Status = StepMotorStatus_Decel;
			}
		} else if (_StepLimit && (_CurStep == _TgtStep)) {
			//当减速流程不存在时，有可能在加速中进入停止流程
			_Status = StepMotorStatus_Stop;
			Stop();
		}
		if (_AccDecUnit->IsDone()) {
			//到达最高步数，开始匀速流程
			_Status = StepMotorStatus_Run;
		}
		SetSpeed(_AccDecUnit->GetCurSpeed());
		break;
	case StepMotorStatus_Run:
		if (_CurStep >= _DecelStartStep) {
			//到达减速步数，进入减速流程
			StartDec();
			_Status = StepMotorStatus_Decel;
		} else if (_StepLimit && (_CurStep == _TgtStep)) {
			//当减速流程不存在时，有可能在运行中中进入停止流程
			_Status = StepMotorStatus_Stop;
			Stop();
		}
		break;
	case StepMotorStatus_Decel:
		SetSpeed(_AccDecUnit->GetCurSpeed());
		if (_CurStep == _TgtStep) {
			//当减速流程时，有可能进入停止流程
			_Status = StepMotorStatus_Stop;
			Stop();
		}
		break;
	default:
		//Error @Romeli 不可能到达位置（内存溢出）
		Debug::Print((uint8_t*) __FILE__, __LINE__, "Unkown error");
		break;
	}
	TIM_CLEAR_UPDATE_FLAG(_TIMx);
}

/*
 * author Romeli
 * explain GPIO初始化（应在派生类中重写）
 * return void
 */
void StepMotor::GPIOInit() {
	/*	GPIO_InitTypeDef GPIO_InitStructure;

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	 //Init pin PUL on TIM2 CH1
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);

	 //Init pin DIR&EN on TIM2 CH2&3
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);

	 GPIOC->BRR = GPIO_Pin_8 | GPIO_Pin_9;*/
}

/*
 * author Romeli
 * explain TIM初始化（应在派生类中重写）
 * return void
 */
void StepMotor::TIMInit() {
	/*	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	 TIM_DeInit(_TIMx);
	 TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	 TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	 TIM_TimeBaseInitStructure.TIM_Prescaler = 7;	//若需更改最小脉冲频率
	 TIM_TimeBaseInitStructure.TIM_Period = (uint16_t) ((SystemCoreClock
	 / STEP_MOTOR_MIN_SPEED) >> 3);
	 TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	 TIM_TimeBaseInit(_TIMx, &TIM_TimeBaseInitStructure);

	 //!!!!非常重要，启动ARR预装载寄存器
	 TIM_ARRPreloadConfig(_TIMx, DISABLE);

	 TIM_OCInitTypeDef TIM_OCInitStructure;

	 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM模式1
	 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //主输出使能
	 TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //关闭副输出
	 TIM_OCInitStructure.TIM_Pulse = (uint16_t) (_TIMx->ARR >> 1); //脉冲宽度设置为50%
	 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_High; //主输出有效电平为高电平
	 TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //副输出有效电平高低电平
	 TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Reset; //主输空闲时为无效电平
	 TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; //副输空闲时为无效电平
	 //使用CH2作为输出
	 TIM_OC2Init(_TIMx, &TIM_OCInitStructure);
	 //!!!!非常重要，启动CCRx预装载寄存器
	 TIM_OC2PreloadConfig(_TIMx, TIM_OCPreload_Disable);
	 //作为高级定时器必须要开启PWM输出
	 TIM_CtrlPWMOutputs(_TIMx, ENABLE);*/
}

/*
 * author Romeli
 * explain IT初始化（应在派生类中重写）
 * return void
 */
void StepMotor::ITInit() {
	/*	NVIC_InitTypeDef NVIC_InitStructure;
	 //设置中断

	 NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
	 StepMotor3_TIM8_UP_IRQn.ITPriority_PreemptionPriority;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority =
	 StepMotor3_TIM8_UP_IRQn.ITPriority_SubPriority;
	 NVIC_Init(&NVIC_InitStructure);*/
}

/*
 * author Romeli
 * explain 设置方向引脚状态（应在派生类中重写）
 * return void
 */
void StepMotor::SetDirPin(FunctionalState newState) {
	if (newState != DISABLE) {

	} else {

	}
}

/*
 * author Romeli
 * explain 设置使能引脚状态（应在派生类中重写）
 * return void
 */
void StepMotor::SetEnPin(FunctionalState newState) {
	if (newState != DISABLE) {

	} else {

	}
}

/*
 * author Romeli
 * explain	检测是否可以安全移动（需要在派生类中重写）
 * return bool 是否安全可移动
 */
bool StepMotor::SafetyCheck() {
	return true;
}

/*
 * author Romeli
 * explain 更改当前步进电机方向
 * param dir 欲改变的方向
 * return void
 */
void StepMotor::SetDir(StepMotorDir_Typedef dir) {
	_Dir = dir;
	if (_Dir == _RelativeDir) {
		SetDirPin(ENABLE);
	} else {
		SetDirPin(DISABLE);
	}
}

/*
 * author Romeli
 * explain 开始减速
 * return void
 */
void StepMotor::StartDec() {	//关闭速度计算单元
	//开始减速计算
	_AccDecUnit->Start(StepMotorAccDecUnitMode_Decel);
}

/*
 * author Romeli
 * explain 计算当前速度减速到怠速需要多少个脉冲
 * param 当前速度
 * return uint32_t
 */
uint32_t StepMotor::GetDecelStep(uint16_t speedFrom) {
	float n = (float) (speedFrom - STEP_MOTOR_MIN_SPEED) / (float) _Decel;
	return (uint32_t) ((float) (speedFrom + STEP_MOTOR_MIN_SPEED) * n) >> 1;
}

/*
 * author Romeli
 * explain 应用速度到脉冲定时器（不会立即更新）
 * param 速度
 * return void
 */
void StepMotor::SetSpeed(uint16_t speed) {
	_TIMx->ARR = (uint16_t) (_TIMy_FRQ / speed);
	*_TIMx_CCRx = (uint16_t) (_TIMx->ARR >> 1);
}

} /* namespace Device */
} /* namespace User */

