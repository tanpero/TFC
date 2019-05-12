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

			class INIException : std::exception {
				int _errno;
				std::string _info;
			public:
				INIException(int errtype, std::string info) : _errno(errtype), _info(info) {}
				~INIException() {}
				inline int errtype()
				{
					return _errno;
				}
				inline std::string info()
				{
					return _info;
				}
			};


			// 分别存储解析得到的值和注释
			struct ININativeValue
			{
				std::string value;
				std::string comment;
			};


			struct INIItem
			{
				std::string key;
				std::string value;
				std::string comment;  // 每个键的注释，都是指该行上方的内容
				std::string rightComment;
			};


			class INISection
			{
			public:
				using INIItemIterator = std::vector<INIItem>::iterator;  // 定义一个迭代器，即指向键元素的指针

				INIItemIterator begin();
				INIItemIterator end();

			private:
				std::string name;
				std::string comment;  // 每个段的注释，都是指该行上方的内容
				std::string rightComment;
				std::vector<INIItem> items;  // 键值项数组，一个段可以有多个键值，所有用vector来储存

			public:
				bool hasKey(std::string key);
				size_t length();

				std::string getName();
				void setName(std::string _name);
				std::string getComment();
				void setComment(std::string _comment);
				std::string getRightComment();
				void setRightComment(std::string _rightComment);

				std::vector<INIItem> getItems();
				void pushItem(INIItem item);

				std::vector<std::pair<std::string, std::string>> getEntries();
				std::vector<std::string> getKeys();
				std::vector<std::string> getValues();
			};


			class INIFile
			{
			public:
				INIFile();
				~INIFile();

			public:

				/*
				* 基本文件操作
				*/

				int load(std::string filePath);
				int save(std::string);
				int saveAs(std::string);

			public:

				/*
				* 获取 INI 文件内容
				*/
				std::string getStringValue(std::string section, std::string key);
				std::string getStringValue(std::string key);
				std::string getStringValueOrDefault(std::string section, std::string key, std::string defaultValue);
				std::string getStringValueOrDefault(std::string key, std::string defaultValue);

				int getIntValue(std::string section, std::string key);
				int getIntValue(std::string key);
				int getIntValueOrDefault(std::string section, std::string key, int defaultValue);
				int getIntValueOrDefault(std::string key, int defaultValue);

				double getDoubleValue(std::string section, std::string key);
				double getDoubleValue(std::string key);
				double getDoubleValueOrDefault(std::string section, std::string key, double defaultValue);
				double getDoubleValueOrDefault(std::string key, double defaultValue);

				bool getBoolValue(std::string section, std::string key);
				bool getBoolValue(std::string key);
				bool getBoolValueOrDefault(std::string section, std::string key, bool defaultValue);
				bool getBoolValueOrDefault(std::string key, bool defaultValue);


				std::string getComment(std::string section, std::string key);
				std::string getRightComment(std::string section, std::string key);


				std::vector<std::string> getKeys(std::string section);
				std::vector<std::string> getValues(std::string section);
				std::vector<std::pair<std::string, std::string>> getEntries(std::string section);
				std::vector<INIItem> getItems(std::string section);
				int getItemsLength(std::string section);
				int getItemsLength(INISection section);

				INISection getSection(std::string section);
				std::vector<INISection> getSections();
				std::vector<INISection> getSections(std::string section, ...);
				std::vector<std::string> getSectionNames();

			public:

				/*
				* 检测 INI 文件是否包含指定内容
				*/

				bool hasSection(std::string section);
				bool hasKey(std::string section, std::string key);
				bool hasKey(std::string key);

			public:

				/*
				* 设置 INI 文件内容
				*/

				// 设置字符串值
				void setStringValue(const std::string section, const std::string key, const std::string value);

				// 设置整数值
				void setIntValue(const std::string section, const std::string key, int value);

				// 设置浮点值
				void setDoubleValue(const std::string section, const std::string key, double value);

				// 设置布尔值
				void setBoolValue(const std::string section, const std::string key, bool value);

				// 设置注释，如果key=""则设置段注释
				void setComment(std::string section, std::string key, std::string comment);

				// 设置行尾注释，如果key=""则设置段的行尾注释
				int setRightComment(std::string section, const std::string key, std::string rightComment);

			public:

				/*
				* 删除 INI 文件内容
				*/

				// 删除段
				void deleteSection(std::string section);

				// 删除特定段的特定项
				void deleteKey(std::string section, std::string key);
				void deleteKey(std::string section, INIItem item);
				void deleteKey(INISection section, INIItem item);
				void deleteKey(INISection section, std::string key);

				//设置注释分隔符，默认为"#"
				void setCommentDelimiter(std::string Delimiter = "#");

			public:
				void customBooleans(std::string b0, std::string b1);

			private:
				ININativeValue getValue(std::string section, std::string key);
				void setValue(std::string section, std::string key, std::string value);
				INISection updateSection(std::string cleanLine, std::string comment, std::string rightComment);
				int addEntry(std::string cleanLine, std::string comment, std::string rightComment, INISection currSection);

			private:
				INIItem parse(std::string line);
				std::pair<std::string, std::string> split(std::string line, std::string seq);

				bool isCommentLine(std::string line);

				std::string trimLeft(std::string line, char c = ' ');
				std::string trimRight(std::string line, char c = ' ');
				std::string trim(std::string line, char c = ' ');

				bool startWith(std::string line, std::string prefix);

				bool compareStringIgnoreCase(std::string a, std::string b);

			private:
				void release();
				std::vector<INISection> sectionsCache;
				std::string path;
				INISection defaultSection;

				std::vector<std::pair<std::string, std::string>> beBooleans;

			private:
				using INISectionIterator = std::vector<INISection>::iterator;

			};


		};
	};
};


#endif // !_TFC_DATA_INI_INI_H_