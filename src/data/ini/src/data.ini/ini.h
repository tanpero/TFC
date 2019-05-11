#pragma once
#ifndef _TFC_DATA_FORMAT_INI_INI_H_
#define _TFC_DATA_FORMAT_INI_INI_H_

#define RET_OK 0

// 没有找到匹配的 `]'
#define ERR_UNMATCHED_BRACKETS 2

// 段为空
#define ERR_SECTION_EMPTY 3

// 段名已经存在
#define ERR_SECTION_ALREADY_EXISTS 4

// 解析key value失败
#define ERR_PARSE_KEY_VALUE_FAILED 5

// 打开文件失败
#define ERR_OPEN_FILE_FAILED 6

// 内存不足
#define ERR_NO_ENOUGH_MEMORY 7

// 没有找到对应的key
#define ERR_NOT_FOUND_KEY 8

// 没有找到对应的section
#define ERR_NOT_FOUND_SECTION 9

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>

namespace tfc
{
	namespace data
	{
		namespace ini
		{


		};
	};
};


#endif // !_TFC_DATA_INI_INI_H_