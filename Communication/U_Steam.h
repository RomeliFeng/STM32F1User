/*
 * U_Steam.h
 *
 *  Created on: 2017年9月29日
 *      Author: Romeli
 */

#ifndef U_STEAM_H_
#define U_STEAM_H_

#include <Tool/U_Convert.h>
#include <U_Debug.h>
#include <U_Misc.h>
#include "cmsis_device.h"

typedef struct _DataSteam_Typedef {
	uint8_t* data;
	volatile uint16_t front;
	volatile uint16_t tail;
	uint16_t size;
	volatile bool busy;
} DataSteam_Typedef;

class U_Steam: public U_Convert {
public:
	U_Steam(uint16_t rxBufSize, uint16_t txBufSize) {
		_RxBuf.size = rxBufSize;
		_RxBuf.data = new uint8_t[_RxBuf.size];
		_RxBuf.front = 0;
		_RxBuf.tail = 0;
		_RxBuf.busy = false;

		_TxBuf.size = txBufSize;
		_TxBuf.data = new uint8_t[_TxBuf.size];
		_TxBuf.front = 0;
		_TxBuf.tail = 0;
		_TxBuf.busy = false;
	}

	virtual ~U_Steam() {
		delete _RxBuf.data;
		delete _TxBuf.data;
	}

	//接口
	/*
	 * author Romeli
	 * explain 通过流写数组
	 * param1 data 数组的首地址
	 * param2 len 数组的长度
	 * return Status_Typedef
	 */
	virtual Status_Typedef Write(uint8_t* data, uint16_t len) = 0;
	//虚函数
	virtual Status_Typedef Write(uint8_t data);

	Status_Typedef Print(uint8_t* str);
	inline Status_Typedef Print(const char* str) {
		return Print((uint8_t*) str);
	}
	Status_Typedef Print(int32_t num, uint8_t base = 10);
	inline Status_Typedef Print(int16_t num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}
	inline Status_Typedef Print(int8_t num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}
	inline Status_Typedef Print(uint32_t num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}
	inline Status_Typedef Print(uint16_t num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}
	inline Status_Typedef Print(uint8_t num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}
	inline Status_Typedef Print(int num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}
	inline Status_Typedef Print(unsigned int num, uint8_t base = 10) {
		return Print((int32_t) num, base);
	}

	Status_Typedef Print(double flo, uint8_t ndigit = 2);
	inline Status_Typedef Print(float flo, uint8_t ndigit = 2) {
		return Print((double) flo, ndigit);
	}

	virtual Status_Typedef Read(uint8_t* data, uint16_t len);
	virtual Status_Typedef Read(uint8_t* data);

	Status_Typedef Peek(uint8_t *data);
	Status_Typedef PeekNextDigital(uint8_t* data, uint8_t ignore,
			bool detectDecimal = false);

	virtual Status_Typedef NextInt(void* num, uint8_t ignore = 0);
	virtual Status_Typedef NextFloat(void* flo, uint8_t ignore = 0);

	virtual uint16_t Available();
	virtual bool IsEmpty(DataSteam_Typedef* steam);
	void Clear();
protected:
	DataSteam_Typedef _RxBuf, _TxBuf;
	Status_Typedef SpInc(DataSteam_Typedef* steam);
	Status_Typedef SpDec(DataSteam_Typedef* steam);
private:
	uint16_t getLen(uint8_t *str);
};

#endif /* U_STEAM_H_ */
