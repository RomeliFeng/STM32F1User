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
	USART(uint16_t rxBufSize, uint16_t txBufSize);
	virtual ~USART();

	void Init(uint32_t baud, uint16_t USART_Parity = USART_Parity_No,
			USART485Status_Typedef rs485Status = USART485Status_Disable,
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

	virtual void GPIOInit(USART485Status_Typedef status);
	virtual void USARTInit(uint32_t baud, uint16_t USART_Parity);
	virtual void DMAInit();
	virtual void ITInit(USARTMode_Typedef mode);

	virtual void RS485DirCtl(USART485Dir_Typedef dir);
private:
	volatile bool _DMATxBusy;
	volatile bool _newFrame;
	DataStack_Typedef _DMARxBuf;
	DataStack_Typedef _TxBuf2;

	USARTMode_Typedef _mode;
	USART485Status_Typedef _rs485Status;

	Status_Typedef DMASend(DataStack_Typedef *stack, DataStack_Typedef *txBuf);
};

} /* namespace Communication */
} /* namespace User*/
#endif /* USART_H_ */
