// TestArgs.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include <tuple>
#include <vector>
#include "regist_to_factory.h"

template<typename...TArgs>
void ComponentFactoryEx(std::string className, TArgs... args)
{
	auto c1 = ComponentFactory<TArgs...>::GetInstance()->Create(className, std::forward<TArgs>(args)...);
}

template <typename F1, size_t... I1, typename F2, size_t... I2>
void DynamicCallEx2(const std::string classname, F1 f1, std::index_sequence<I1...>, F2 f2, std::index_sequence<I2...>)
{
	ComponentFactoryEx(classname, f1(I1)..., f2(I2)...);
}

template <size_t N1, size_t N2, typename F1, typename F2>
void DynamicCallEx1(const std::string classname, F1 f1, F2 f2)
{
	DynamicCallEx2(classname, f1, std::make_index_sequence<N1>{}, f2, std::make_index_sequence<N2>{});
}

template<int ICount, int SCount>
struct DynamicCall
{
	static void Invoke(const std::string classname, const std::vector<int>& intargs, const std::vector<std::string>& strargs)
	{
		if (intargs.size() > ICount)
		{
			return DynamicCall<ICount + 1, SCount>().Invoke(classname, intargs, strargs);
		}

		if (strargs.size() > SCount)
		{
			return DynamicCall<ICount, SCount + 1>().Invoke(classname, intargs, strargs);
		}

		DynamicCallEx1<ICount, SCount>(classname, [intargs](size_t i) { return intargs[i]; }, [strargs](size_t i) { return strargs[i]; });
	}
};

template<int SCount>
struct DynamicCall<5, SCount>
{
	static void Invoke(const std::string classname, const std::vector<int>& intargs, const std::vector<std::string>& strargs)
	{
		return;
	}
};

template<int ICount>
struct DynamicCall<ICount, 5>
{
	static void Invoke(const std::string classname, const std::vector<int>& intargs, const std::vector<std::string>& strargs)
	{
		return;
	}
};

int mainx()
{

	RegistToFactory<C1, std::string, std::string>();
	RegistToFactory<C2, int, std::string>();
	RegistToFactory<C3>();

	std::vector<int> ints;
	std::vector<std::string> strs;

	strs.push_back("to c1");
	strs.push_back(" to c1");
	DynamicCall<0, 0>().Invoke(typeid(C1).name(), ints, strs);

	strs.clear();
	ints.push_back(2);
	strs.push_back("to c2");
	DynamicCall<0, 0>().Invoke(typeid(C2).name(), ints, strs);

	ints.clear();
	strs.clear();

	DynamicCall<0, 0>().Invoke(typeid(C3).name(), ints, strs);

	return 0;
}