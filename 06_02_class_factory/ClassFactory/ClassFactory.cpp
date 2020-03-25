// ClassFactory.cpp: 定义控制台应用程序的入口点。
//

#include "ComponentFactory.h"
#include "RegistCreator.h"
#include "Test1.h"

template<typename ...Targs>
Component* CreateComponentr(const std::string& strTypeName, Targs&& ... args) {
    Component* p = ComponentFactory<Targs...>::Instance()->Create(strTypeName, std::forward<Targs>(args)...);
    return(p);
}

int main() {

    RegistCreator<Test1, std::string>();
    RegistCreator<Test2, int>();

    //ComponentFactory<std::string>::Instance()->Regist(typeid(Test1).name(), Test1::CreateObject);

    CreateComponentr(typeid(Test1).name(), std::string("Test1"));
    CreateComponentr(typeid(Test2).name(), 2);

    return 0;
}
