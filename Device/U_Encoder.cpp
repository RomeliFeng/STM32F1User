/*
 * U_Encoder.cpp
 *
 *  Created on: 2017年11月1日
 *      Author: Romeli
 */

#include <Device/U_Encoder.h>

U_Encoder* U_Encoder::_Pool[4];
uint8_t U_Encoder::_PoolSp = 0;

U_Encoder::U_Encoder(TIM_TypeDef* TIMx) {
	_TIMx = TIMx;
	_ExCNT = 0;

	//自动将对象指针加入资源池
	_Pool[_PoolSp++] = this;
}

U_Encoder::~U_Encoder() {
}

/*
 * author Romeli
 * param void
 * return void
 */
void U_Encoder::Init() {
	GPIOInit();
	TIMInit();
	ITInit();
	//顺序很重要
	TIM_CLEAR_UPDATE_FLAG(_TIMx);
	TIM_ENABLE_IT_UPDATE(_TIMx);
	TIM_ENABLE(_TIMx);
}

/*
 * author Romeli
 * explain 初始化所有编码器模块
 * return void
 */
void U_Encoder::InitAll() {
	//初始化池内所有单元
	for (uint8_t i = 0; i < _PoolSp; ++i) {
		_Pool[i]->Init();
	}
	if (_PoolSp == 0) {
		//Error @Romeli 无编码器模块
		U_DebugOut("There have encoder module exsit");
	}
}

void U_Encoder::Set(int32_t pos) {
	if (pos >= 0) {
		_ExCNT = pos / 0x10000;
		_TIMx->CNT = pos - (_ExCNT * 0x10000);
	} else {
		pos = -pos;
		_ExCNT = pos / 0x10000 + 1;
		_TIMx->CNT = (_ExCNT * 0x10000) - pos;
	}
}

int32_t U_Encoder::Get() const {
	return ((int32_t) _ExCNT * 0x10000) + _TIMx->CNT;
}

void U_Encoder::IRQ() {
	if (_TIMx->CNT <= 0x7fff) {
		++_ExCNT;
	} else {
		--_ExCNT;
	}
	TIM_CLEAR_UPDATE_FLAG(_TIMx);
}

void U_Encoder::GPIOInit() {
	U_DebugOut("This function should be override");
	/*	GPIO_InitTypeDef GPIO_InitStructure;

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);*/
}

void U_Encoder::ITInit() {
	U_DebugOut("This function should be override");
	/*	NVIC_InitTypeDef NVIC_InitStructure;

	 NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
	 Encoder2_TIM1_UP_IRQn.ITPriority_PreemptionPriority;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority =
	 Encoder2_TIM1_UP_IRQn.ITPriority_SubPriority;
	 NVIC_Init(&NVIC_InitStructure);

	 TIM_ClearITPendingBit(_TIMx, TIM_IT_Update);
	 TIM_ITConfig(_TIMx, TIM_IT_Update, ENABLE);*/
}

void U_Encoder::TIMInit() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIMRCCInit();

	TIM_DeInit(_TIMx);
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_Period = 0xffff;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(_TIMx, &TIM_TimeBaseInitStructure);

	TIM_EncoderInterfaceConfig(_TIMx, TIM_EncoderMode_TI12,
	TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);

	TIM_ICInitTypeDef TIM_ICInitStructure;

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 1;
	TIM_ICInit(_TIMx, &TIM_ICInitStructure);
	_TIMx->CNT = 0;
}

