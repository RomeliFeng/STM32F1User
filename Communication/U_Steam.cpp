/*
 * U_Steam.cpp
 *
 *  Created on: 2017年9月29日
 *      Author: Romeli
 */

#include <Communication/U_Steam.h>

/*
 * author Romeli
 * explain 获取缓冲区内的缓冲字符数量
 * return uint16_t
 */
uint16_t U_Steam::Available() {
	return uint16_t(
			_RxBuf.front <= _RxBuf.tail ?
			_RxBuf.tail - _RxBuf.front :
					_RxBuf.size - _RxBuf.front + _RxBuf.tail);
}

/*
 * author Romeli
 * explain 通过流写单字节
 * param data 数据
 * return Status_Typedef
 */
inline Status_Typedef U_Steam::Write(uint8_t data) {
	return Write(&data, 1);
}

/*
 * author Romeli
 * explain 通过流写字符串
 * param 字符串地址
 * return Status_Typedef
 */
Status_Typedef U_Steam::Print(uint8_t* str) {
	uint16_t len = getLen((uint8_t *) str);
	if (len != 0) {
		return Write((uint8_t *) str, len);
	} else {
		U_DebugOut("Error @Romeli 空数组");
		return Status_Error;
	}
}

/*
 * author Romeli
 * explain 通过流打印整形数
 * param1 num 整形数
 * param2 base 进制
 * return Status_Typedef
 */
Status_Typedef U_Steam::Print(int32_t num, uint8_t base) {
	uint8_t str[32];
	uint16_t len = byNumber(num, base, str);
	return Write(str, len);
}

/*
 * author Romeli
 * explain 通过流打印浮点数
 * param1 flo 浮点数
 * param2 ndigit 小数位数
 * return Status_Typedef
 */
Status_Typedef U_Steam::Print(double flo, uint8_t ndigit) {
	uint8_t str[32];
	uint16_t len = byFloat(flo, ndigit, str);
	return Write(str, len);
}

/*
 * author Romeli
 * explain 从流中读取指定数据长度
 * param1 data 读回数据存放数组
 * param2 len 读取数据长度
 * return Status_Typedef
 */
Status_Typedef U_Steam::Read(uint8_t* data, uint16_t len) {
	//循环读取
	for (uint8_t i = 0; i < len; ++i) {
		Read(data + i);
	}
	return Status_Ok;
}

/*
 * author Romeli
 * explain 从流中读取一个字节
 * param1 data 读回数据存放位置
 * return Status_Typedef
 */
Status_Typedef U_Steam::Read(uint8_t* data) {
	//读取一个数
	*data = _RxBuf.data[_RxBuf.front];
	return SpInc(&_RxBuf);
}

/*
 * author Romeli
 * explain 偷看一个字节（不移动缓冲指针）
 * param data 读回数据存放位置
 * return Status_Typedef
 */
Status_Typedef U_Steam::Peek(uint8_t* data) {
	//偷看一个数
	*data = _RxBuf.data[_RxBuf.front];
	return Status_Ok;
}

/*
 * author Romeli
 * explain 偷看下一个数字
 * param1 data 读取的数字存放位置
 * param2 ignore 忽略的字符
 * return Status_Typedef
 */
Status_Typedef U_Steam::PeekNextDigital(uint8_t *data, uint8_t ignore,
		bool detectDecimal) {
	//偷看一个数
	Peek(data);
	//当读到的字符为 '-','0'-'9','.'（detectDecimal为true）时返回
	if ((*data == '-') || ((*data >= '0') && (*data <= '9'))
			|| ((*data == '.') && detectDecimal) || (*data == ignore)) {
	} else {
		return Status_Error;
	}
	return Status_Ok;
}

/*
 * author Romeli
 * explain 从流中读取一个整形数
 * param1 num 读取的整数存放位置
 * param2 ignore 忽略的字符
 * return Status_Typedef
 */
Status_Typedef U_Steam::NextInt(void *num, uint8_t ignore) {
	bool firstChar = true;
	bool isNeg = false;
	uint8_t c = 0;
	uint16_t sp = _RxBuf.front;
	int32_t n = 0;

	while (Available() > 0) {
		if (PeekNextDigital(&c, ignore, false) == Status_Ok) {
			//如果读到数字
			if (c == '-') {
				if (firstChar) {
					//检测到一个'-'
					isNeg = true;
					SpInc(&_RxBuf);
					continue;
				} else {
					//'-'不是第一个数
					break;
				}
			} else if ((c == ignore) && (ignore != 0)) {
				SpInc(&_RxBuf);
				continue;
			}
			n = n * 10 + c - '0';
			firstChar = false;
			SpInc(&_RxBuf);
		} else {
			break;
		}
	}
	if ((sp != _RxBuf.front) && (c != '-') && (c != ignore)) {
		//有读取到数
		if (isNeg) {
			n = -n;
		}
		*(int32_t *) num = n;
		return Status_Ok;
	} else {
		//没有读取到数
		*(int32_t *) num = 0;
		return Status_Error;
	}
}

/*
 * author Romeli
 * explain 从流中读取一个浮点数
 * param1 flo 读取的浮点数存放位置
 * param2 ignore 忽略的字符
 * return Status_Typedef
 */
Status_Typedef U_Steam::NextFloat(void* flo, uint8_t ignore) {
	double f = 0;
	double frac = 1.0;
	bool isNeg = false;
	bool isFra = false;
	bool firstChar = true;
	uint16_t sp = _RxBuf.front;
	uint8_t c = 0;

	while (Available() > 0) {
		if (PeekNextDigital(&c, ignore, true) == Status_Ok) {
			if (c == '-') {
				if (firstChar) {
					//检测到一个'-'
					isNeg = true;
					SpInc(&_RxBuf);
					continue;
				} else {
					//'-'不是第一个数
					break;
				}
			} else if ((c == ignore) && (ignore != 0)) {
				SpInc(&_RxBuf);
				continue;
			} else if (c == '.') {
				if (isFra) { //不应出现两个'-'
					break;
				} else {
					if (!firstChar) {
						SpInc(&_RxBuf);
						isFra = true;
						continue;
					} else {
						//第一个字符为点的时候
						break;
					}
				}
			}
			if (isFra) {
				frac *= 0.1;
			}
			f = f * 10 + c - '0';
			SpInc(&_RxBuf);
			firstChar = false;
		} else {
			break;
		}
	}

	if ((sp != _RxBuf.front) && (c != '-') && (c != ignore)) {
		//有读取到数
		f = isNeg ? -f : f;
		f = isFra ? f * frac : f;
		*(double *) flo = f;
		return Status_Ok;
	} else {
		//没有读取到数
		*(double *) flo = 0;
		return Status_Error;
	}
}

/*
 * author Romeli
 * explain 检查站内是否有数据
 * param steam 栈地址
 * return bool
 */
inline bool U_Steam::IsEmpty(DataSteam_Typedef* steam) {
	//判断缓冲区是否为空
	return steam->front == steam->tail;
}

/*
 * author Romeli
 * explain 向下移动流指针
 * param1 steam 流指针
 * return Status_Typedef
 */
Status_Typedef U_Steam::SpInc(DataSteam_Typedef *steam) {
	if (IsEmpty(steam)) {
		//缓冲区为空
		return Status_Error;
	} else {
		//缓冲区指针+1
		steam->front = uint16_t((steam->front + 1) % steam->size);
		return Status_Ok;
	}
}

/*
 * author Romeli
 * explain 向上移动流指针
 * param1 steam 流指针
 * return Status_Typedef
 */
Status_Typedef U_Steam::SpDec(DataSteam_Typedef* steam) {
	steam->front = uint16_t(steam->front == 0 ? steam->size : steam->front - 1);
	return Status_Ok;
}

/*
 * author Romeli
 * explain 清空读取流内数据
 * return void
 */
void U_Steam::Clear() {
	_RxBuf.front = _RxBuf.tail;
}

/*
 * author Romeli
 * explain 计算字符串长度
 * param str 字符串地址
 * return uint16_t
 */
uint16_t U_Steam::getLen(uint8_t* str) {
	uint16_t len = 0;
	for (len = 0; *(str + len) != '\0'; ++len)
		;
	return len;
}
