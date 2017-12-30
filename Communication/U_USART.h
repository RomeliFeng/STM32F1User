/*
 * U_USART.h
 *
 *  Created on: 2017年9月27日
 *      Author: Romeli
 */

#ifndef USART_H_
#define USART_H_

#include <Communication/U_Steam.h>
#include <Communication/U_Steam.h>
#include <U_Debug.h>
#include "cmsis_device.h"
#include "ITPriority.h"

typedef enum {
	USARTMode_Normal, USARTMode_DMA
} USARTMode_Typedef;

typedef enum {
	USARTRS485Status_Enable, USARTRS485Status_Disable
} USARTRS485Status_Typedef;

typedef enum {
	USARTRS485Dir_Tx, USARTRS485Dir_Rx
} USARTRS485Dir_Typedef;

class U_USART: public U_Steam {
public:
	void (*ReceiveEvent)();

	U_USART(uint16_t rxBufSize, uint16_t txBufSize, USART_TypeDef* USARTx,
			DMA_TypeDef* DMAx, DMA_Channel_TypeDef* DMAy_Channelx_Rx,
			DMA_Channel_TypeDef* DMAy_Channelx_Tx);
	virtual ~U_USART();

	void Init(uint32_t baud, uint16_t USART_Parity = USART_Parity_No,
			USARTRS485Status_Typedef RS485Status = USARTRS485Status_Disable,
			USARTMode_Typedef mode = USARTMode_DMA);

	Status_Typedef Write(uint8_t* data, uint16_t len);

	bool CheckFrame();

	Status_Typedef IRQUSART();
	Status_Typedef IRQDMATx();
protected:
	USART_TypeDef *_USARTx;
	DMA_TypeDef *_DMAx;
	DMA_Channel_TypeDef *_DMAy_Channelx_Rx, *_DMAy_Channelx_Tx;
	uint32_t _DMA_IT_TC_TX;

	virtual void USARTRCCInit() = 0;
	virtual void DMARCCInit() = 0;
	virtual void GPIOInit() = 0;
	virtual void ITInit() = 0;
	virtual void RS485DirCtl(USARTRS485Dir_Typedef dir) = 0;
private:
	volatile bool _DMATxBusy;
	volatile bool _newFrame;
	DataSteam_Typedef _DMARxBuf;
	DataSteam_Typedef _TxBuf2;

	USARTMode_Typedef _mode;
	USARTRS485Status_Typedef _RS485Status;

	void USARTInit(uint32_t baud, uint16_t USART_Parity);
	void ITInit(USARTMode_Typedef mode);
	void DMAInit();

	Status_Typedef DMASend(DataSteam_Typedef *stack, DataSteam_Typedef *txBuf);
};

#endif /* USART_H_ */
