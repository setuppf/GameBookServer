#pragma once
#include <iostream>

class Test1 : public Component {
public:
    static Test1* CreateObject(std::string&& p1) {
        return new Test1(p1);
    }

    Test1(const std::string p1) {
        std::cout << "create test1. p1:" << p1.c_str() << std::endl;
    }
};

class Test2 : public Component {
public:
    Test2(const int p1) {
        std::cout << "create test2. p2:" << p1 << std::endl;
    }
};
