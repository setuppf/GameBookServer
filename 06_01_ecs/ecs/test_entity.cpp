#include "test_entity.h"
#include <iostream>

void TestEntityWithInitAndUpdate::Initialize()
{
    std::cout << typeid(this).name() << "::" << "Initialize" << std::endl;
}

void TestEntityWithInitAndUpdate::Update()
{
    if (_isShow)
        return;

    std::cout << typeid(this).name() << "::" << "Update" << std::endl;
    _isShow = true;
}

void TestEntityWithUpdate::Update()
{
    if (_isShow)
        return;

    std::cout << typeid(this).name() << "::" << "Update" << std::endl;
    _isShow = true;
}
