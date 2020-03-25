#pragma once

#include <iostream>

class Component
{
};

class C1 :public Component
{
public:
	C1(std::string p1, std::string p2)
	{
		std::cout << "call c1." << " string1:" << p1 << " string2:" << p2 << std::endl;
	}
};

class C2 :public Component
{
public:
	C2(int i1, std::string p1)
	{
		std::cout << "call c2." << " int value:" << i1 << " string value:" << p1 << std::endl;
	}
};

class C3 :public Component
{
public:
	C3()
	{
		std::cout << "call c3" << std::endl;
	}
};