#pragma once
#ifndef _TFC_DATA_FORMAT_INI_INI_H_
#define _TFC_DATA_FORMAT_INI_INI_H_

#define RET_OK 0

// û���ҵ�ƥ��� `]'
#define ERR_UNMATCHED_BRACKETS 2

// ��Ϊ��
#define ERR_SECTION_EMPTY 3

// �����Ѿ�����
#define ERR_SECTION_ALREADY_EXISTS 4

// ����key valueʧ��
#define ERR_PARSE_KEY_VALUE_FAILED 5

// ���ļ�ʧ��
#define ERR_OPEN_FILE_FAILED 6

// �ڴ治��
#define ERR_NO_ENOUGH_MEMORY 7

// û���ҵ���Ӧ��key
#define ERR_NOT_FOUND_KEY 8

// û���ҵ���Ӧ��section
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