#include "test_c_init.h"
#include <iostream>

void TestCInit::Initialize()
{
    std::cout << typeid(this).name() << "::" << "Initialize" << std::endl;
}

void TestCInit::Dispose()
{

}
