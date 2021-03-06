// ini-test.cpp: 定义控制台应用程序的入口点。
//
#define _DEBUG_DATA_INI_
#include "stdafx.h"
#include "../../src/data.ini/ini.h"
#define in(s) std::cout << s << std::endl;

int main()
{
	using namespace tfc::data::ini;

	INIFile file;
	try
	{
		debug();
		in("--before file.load ------")
			file.load("test.ini");
		in("--after file.load -----");
		std::cout << file.getSection("hello").getItems().at(0).value << std::endl;
		in("--- file.")
		debug();
		std::cout << file.getStringValue("hello", "hi") << std::endl;
	}
	catch (INIException& e)
	{
		file.print();
		std::cout << e.info() << std::endl;
	}
	
    return 0;
}

