/*
 * U_USART.h
 *
 *  Created on: 2017年9月27日
 *      Author: Romeli
 */

#ifndef USART_H_
#define USART_H_

#include <Communication/U_Steam.h>
#include <U_Debug.h>
#include "U_Misc.h"
#include "cmsis_device.h"
#include "functional"

class U_USART: public U_Steam {
public:

	enum Mode_Typedef {
		Mode_Interrupt, Mode_DMA
	};

	enum RS485Status_Typedef {
		RS485Status_Enable, RS485Status_Disable
	};

	enum RS485Dir_Typedef {
		RS485Dir_Tx, RS485Dir_Rx
	};

	std::function<void(void)> ReceiveEvent;

	U_USART(uint16_t rxBufSize, uint16_t txBufSize, USART_TypeDef* USARTx,
			U_IT_Typedef itUSARTX);
	U_USART(uint16_t rxBufSize, uint16_t txBufSize, USART_TypeDef* USARTx,
			U_IT_Typedef itUSARTx, DMA_TypeDef* DMAx,
			DMA_Channel_TypeDef* DMAy_Channelx_Rx,
			DMA_Channel_TypeDef* DMAy_Channelx_Tx, U_IT_Typedef itDMAxRx,
			U_IT_Typedef itDMAxTx);
	virtual ~U_USART();

	void Init(uint32_t baud, uint16_t USART_Parity = USART_Parity_No,
			RS485Status_Typedef RS485Status = RS485Status_Disable);

	Status_Typedef Write(uint8_t* data, uint16_t len);

	bool CheckFrame();

	Status_Typedef IRQUSART();
	Status_Typedef IRQDMATx();
protected:
	USART_TypeDef *_USARTx;
	DMA_TypeDef *_DMAx;
	DMA_Channel_TypeDef *_DMAy_Channelx_Rx;
	DMA_Channel_TypeDef *_DMAy_Channelx_Tx;
	uint32_t _DMA_IT_TC_TX;

	virtual void USARTRCCInit() = 0;
	virtual void DMARCCInit() = 0;
	virtual void GPIOInit() = 0;
	virtual void RS485DirCtl(RS485Dir_Typedef dir);
private:
	volatile bool _DMATxBusy = false;
	volatile bool _newFrame = false;
	RS485Status_Typedef _RS485Status = RS485Status_Disable;

	DataSteam_Typedef _DMARxBuf;
	DataSteam_Typedef _DMATxBuf;

	U_IT_Typedef _ITUSARTx;
	U_IT_Typedef _ITDMAxRx;
	U_IT_Typedef _ITDMAxTx;

	Mode_Typedef _mode;

	void CalcDMATC();

	void USARTInit(uint32_t baud, uint16_t USART_Parity);
	void ITInit(Mode_Typedef mode);
	void DMAInit();

	Status_Typedef DMASend(DataSteam_Typedef *stack, DataSteam_Typedef *txBuf);
};

#endif /* USART_H_ */
