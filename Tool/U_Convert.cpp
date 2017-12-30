/*
 * U_Convert.cpp
 *
 *  Created on: 2016��10��6��
 *      Author: Romeli
 */

#include <Tool/U_Convert.h>

/*
 * author Romeli
 * explain 将整形数转换为字符串
 * param1 num 整形数
 * param2 base 进制
 * param3 str 目标字符串地址
 * return uint8_t
 */
uint8_t U_Convert::byNumber(int32_t num, uint8_t base, uint8_t* str) {
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

/*
 * author Romeli
 * explain 将浮点数转换为字符串
 * param1 flo 浮点数
 * param2 ndigit 小数位数
 * param3 str 目标字符串地址
 * return uint8_t
 */
uint8_t U_Convert::byFloat(double flo, uint8_t ndigit, uint8_t* str) {
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

/*
 * author Romeli
 * explain 根据进制计算整形数长度
 * param1 num 整形数
 * param2 base 进制
 * return uint8_t
 */
uint8_t U_Convert::getLen(uint32_t num, uint8_t base) {
	uint8_t i;
	if (num == 0) {
		return 1;
	}
	for (i = 0; num != 0; ++i) {
		num /= base;
	}
	return i;
}

/*
 * author Romeli
 * explain 求10的n次方
 * param power 次方数
 * return uint32_t
 */
uint32_t U_Convert::pow10(uint8_t power) {
	if (power == 0)
		return 1;
	else
		return 10 * pow10(--power);
}

/*
 * author Romeli
 * explain 两个字符串拼接
 * param1 str_to 目标字符串
 * param2 str_to_len 目标字符串长度
 * param3 str_from 被拼接的字符串
 * param4 str_from_len 被拼接的字符串长度
 * return uint8_t
 */
uint8_t U_Convert::strcat(uint8_t* str_to, uint8_t str_to_len,
		uint8_t* str_from,
		uint8_t str_from_len) {
	uint8_t i;
	for (i = 0; i < str_from_len; ++i) { //搬移数据
		str_to[str_to_len + i] = str_from[i];
	}
	str_to[str_to_len + i] = '\0'; //在字符串末尾填'\0'
	return (uint8_t) (str_to_len + str_from_len); //返回字符串长度
}
