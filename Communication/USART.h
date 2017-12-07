/*
 * USART.h
 *
 *  Created on: 2017年9月27日
 *      Author: Romeli
 */

#ifndef USART_H_
#define USART_H_

#include <Communication/Steam.h>
#include "cmsis_device.h"
#include "ITPriority.h"
#include "Steam.h"
#include "Debug.h"

namespace User {
namespace Communication {

typedef enum {
	USARTMode_Normal, USARTMode_DMA
} USARTMode_Typedef;

typedef enum {
	USART485Status_Enable, USART485Status_Disable
} USART485Status_Typedef;

typedef enum {
	USART485Dir_Tx, USART485Dir_Rx
} USART485Dir_Typedef;

class USART: public Steam {
public:
	USART(uint16_t rxBufSize, uint16_t txBufSize, USART_TypeDef* USARTx,
			DMA_TypeDef* DMAx, DMA_Channel_TypeDef* DMAy_Channelx_Rx,
			DMA_Channel_TypeDef* DMAy_Channelx_Tx);
	virtual ~USART();

	void Init(uint32_t baud, uint16_t USART_Parity = USART_Parity_No,
			USART485Status_Typedef RS485Status = USART485Status_Disable,
			USARTMode_Typedef mode = USARTMode_DMA);

	Status_Typedef Write(uint8_t* data, uint16_t len);

	bool CheckFrame();

	void ReceiveEvent();

	Status_Typedef USARTIRQ();
	Status_Typedef DMATxIRQ();
protected:
	USART_TypeDef *_USARTx;
	DMA_TypeDef *_DMAx;
	DMA_Channel_TypeDef *_DMAy_Channelx_Rx, *_DMAy_Channelx_Tx;
	uint32_t _DMA_IT_TC_TX;

	virtual void USARTRCCInit() = 0;
	virtual void DMARCCInit() = 0;
	virtual void GPIOInit() = 0;
	virtual void ITInit(USARTMode_Typedef mode) = 0;
	virtual void RDPinCtl(BitAction BitVal) = 0;
private:
	volatile bool _DMATxBusy;
	volatile bool _newFrame;
	DataSteam_Typedef _DMARxBuf;
	DataSteam_Typedef _TxBuf2;

	USARTMode_Typedef _mode;
	USART485Status_Typedef _RS485Status;

	void USARTInit(uint32_t baud, uint16_t USART_Parity);
	void DMAInit();

	Status_Typedef DMASend(DataSteam_Typedef *stack, DataSteam_Typedef *txBuf);
	void RS485DirCtl(USART485Dir_Typedef dir);
};

} /* namespace Communication */
} /* namespace User*/
#endif /* USART_H_ */
