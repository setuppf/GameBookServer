#include "test_c_update.h"
#include <iostream>

void TestCUpdate::Dispose()
{
}

void TestCUpdate::Update()
{
    if (_isShow)
        return;

    std::cout << typeid(this).name() << "::" << "Update" << std::endl;
    _isShow = true;
}
