/*
 * Encoder.cpp
 *
 *  Created on: 2017年11月1日
 *      Author: Romeli
 */

#include <Device/Encoder.h>

namespace User {
namespace Device {

#define TIMEC_Enable (_TIMx->CR1 |= TIM_CR1_CEN)
#define TIMEC_Disable (_TIMx->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN)))

Encoder::Encoder() {
	_TIMx = TIM1;
	_ExCNT = 0;
}

Encoder::~Encoder() {
}

/*
 * author Romeli
 * param void
 * return void
 */
void Encoder::Init() {
	GPIOInit();
	TIMInit();
	ITInit();
	TIMEC_Enable;
}

void Encoder::Set(int32_t pos) {
	TIMEC_Disable;
	if (pos >= 0) {
		_ExCNT = pos / 0x10000;
		_TIMx->CNT = pos - (_ExCNT * 0x10000);
	} else {
		pos = -pos;
		_ExCNT = pos / 0x10000 + 1;
		_TIMx->CNT = (_ExCNT * 0x10000) - pos;
	}
	TIMEC_Enable;
}

int32_t Encoder::Get() const {
	return ((int32_t) _ExCNT * 0x10000) + _TIMx->CNT;
}

void Encoder::IRQAction() {
	if (_TIMx->CNT <= 0x7fff) {
		++_ExCNT;
	} else {
		--_ExCNT;
	}
	_TIMx->SR = (uint16_t) ~TIM_IT_Update;
}

void Encoder::GPIOInit() {
	DebugOut("This function should be override");
	/*	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);*/
}

void Encoder::TIMInit() {
	DebugOut("This function should be override");
	/*	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

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
	 _TIMx->CNT = 0;*/
}

void Encoder::ITInit() {
	DebugOut("This function should be override");
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

} /* namespace Device */
} /* namespace User*/
