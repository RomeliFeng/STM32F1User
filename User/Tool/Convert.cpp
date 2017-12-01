/*
 * Convert.cpp
 *
 *  Created on: 2016��10��6��
 *      Author: Romeli
 */

#include "Convert.h"

namespace User {
namespace Tool {

uint8_t Convert::byNumber(int32_t num, uint8_t base, uint8_t* str) {
	uint8_t len = 0;
	//小于零取反加负号
	if (num < 0) {
		num = -num;
		*str++ = '-';
		++len;
	}

	//按照进制取得长度
	len = (uint8_t) (len + getLen((uint32_t) num, base));
	str += len;
	*str = '\0';
	do {
		uint8_t c = (uint8_t) (num % base);
		num /= base;
		*--str = c < 10 ? (uint8_t) (c + '0') : (uint8_t) (c + 'A' - 10);
	} while (num);
	return len;
}

uint8_t Convert::byFloat(double flo, uint8_t ndigit, uint8_t* str) {
	uint8_t len = 0, len2 = 0;
	uint32_t int_part = 0;
	double rem_part = 0;

	uint8_t dot = '.';

	uint8_t tmp[32];
	//小于零取反加负号
	if (flo < 0.0) {
		str[len] = '-';
		flo = -flo;
		++len; //字符串长度+1 ‘-’号长度
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	float rounding = 0.5f;
	for (uint8_t i = 0; i < ndigit; ++i)
		rounding /= 10.0f;

	flo += rounding;

	// Extract the integer part of the number and print it
	int_part = (uint32_t) flo;
	rem_part = (flo - (double) int_part);

	len2 = byNumber(int_part, 10, tmp); //转换整数部分
	len = strcat(str, len, tmp, len2); //拼接整数部分到字符串
	if (ndigit > 0) { //如果有小数部分
		len = strcat(str, len, &dot, 1);
		while (ndigit--) {
			rem_part *= 10;
			int_part = (int16_t) rem_part;	//每次转换一位小数
			rem_part -= int_part;
			str[len++] = (uint8_t) (int_part + 0x30);
		}
	}
	return len;
}

uint8_t Convert::getLen(uint32_t num, uint8_t base) {
	uint8_t i;
	if (num == 0) {
		return 1;
	}
	for (i = 0; num != 0; ++i) {
		num /= base;
	}
	return i;
}

double Convert::pow10(uint8_t power) {
	if (power == 0)
		return 1.0;
	else
		return 10.0 * pow10(--power);
}

uint8_t Convert::strcat(uint8_t* str_to, uint8_t str_to_len, uint8_t* str_from,
		uint8_t str_from_len) {
	uint8_t i;
	for (i = 0; i < str_from_len; ++i) { //搬移数据
		str_to[str_to_len + i] = str_from[i];
	}
	str_to[str_to_len + i] = '\0'; //在字符串末尾填'\0'
	return (uint8_t) (str_to_len + str_from_len); //返回字符串长度
}

} /* namespace Device */
} /* namespace Tool*/
