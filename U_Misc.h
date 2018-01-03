/*
 * U_Misc.h
 *
 *  Created on: 2017��1��8��
 *      Author: Romeli
 */

#ifndef U_MISC_H_
#define U_MISC_H_

#include "cmsis_device.h"

//these function are work for high speed setting
inline void TIM_Enable(TIM_TypeDef* TIMx) {
	TIMx->CR1 |= TIM_CR1_CEN;
}

inline void TIM_Disable(TIM_TypeDef* TIMx) {
	TIMx->CR1 &= (uint16_t) (~((uint16_t) TIM_CR1_CEN));
}

inline void TIM_Clear_Update_Flag(TIM_TypeDef* TIMx) {
	TIMx->SR = (uint16_t) ~TIM_IT_Update;
}

inline void TIM_PSC_Reload(TIM_TypeDef* TIMx) {
	TIMx->EGR = TIM_PSCReloadMode_Immediate;
}

inline void TIM_Enable_IT_Update(TIM_TypeDef* TIMx) {
	TIMx->DIER |= TIM_IT_Update;
}

inline void TIM_Disable_IT_Update(TIM_TypeDef* TIMx) {
	TIMx->DIER &= (uint16_t) ~TIM_IT_Update;
}

typedef struct _Bit_Typedef {
	uint8_t bit0 :1;
	uint8_t bit1 :1;
	uint8_t bit2 :1;
	uint8_t bit3 :1;
	uint8_t bit4 :1;
	uint8_t bit5 :1;
	uint8_t bit6 :1;
	uint8_t bit7 :1;
} Bit_Typedef;

typedef union _BytetoBit_Typedef {
	uint8_t byte;
	Bit_Typedef bit;
} BytetoBit_Typedef;

typedef union _WordtoByte_Typedef {
	uint8_t byte[2];
	uint16_t word;
} WordtoByte_Typedef;

typedef union _WordtoByteSigned_Typedef {
	uint8_t byte[2];
	int16_t word;
} WordtoByteSigned_Typedef;

typedef union _TwoWordtoByte_Typedef {
	uint8_t byte[4];
	uint32_t twoword;
} TwoWordtoByte_Typedef;

typedef union _TwoWordtoByteSigned_Typedef {
	uint8_t byte[4];
	int32_t twoword;
} TwoWordtoByteSigned_Typedef;

typedef union _DoubletoByte_Typedef {
	double d;
	uint8_t byte[8];
} DoubletoByte_Typedef;

typedef enum _Status_Typedef {
	Status_Ok, Status_Error, Status_TimeOut
} Status_Typedef;

#endif /* U_MISC_H_ */
