#include <tuple>
#include <tuple>
#include "regist_to_factory.h"
#include <list>
#include <vector>

struct DataInfo
{
	bool IsInt;
	int IntVal;
	std::string StrVal;

	DataInfo(const int value)
	{
		IsInt = true;
		IntVal = value;
		StrVal = "";
	}

	DataInfo(const std::string value)
	{
		IsInt = false;
		IntVal = 0;
		StrVal = value;
	}
};

template <typename... TArgs, size_t... Index>
void ComponentFactoryEx(std::string className, const std::tuple<TArgs...>& args, std::index_sequence<Index...>)
{
	auto c1 = ComponentFactory<TArgs...>::GetInstance()->Create(className, std::get<Index>(args)...);
}

template<int ICount>
struct DynamicCall
{
	template<typename...TArgs>
	static void Invoke(std::string className, std::tuple<TArgs...> t1, std::list<DataInfo>& params)
	{
		if (params.size() == 0)
		{
			ComponentFactoryEx(className, t1, std::make_index_sequence<sizeof...(TArgs)>());
			return;
		}

		const DataInfo info = (*params.begin());
		params.pop_front();

		if (info.IsInt)
		{
			auto t2 = std::tuple_cat(t1, std::make_tuple(info.IntVal));
			DynamicCall<ICount - 1>::Invoke(className, t2, params);
		}
		else
		{
			auto t2 = std::tuple_cat(t1, std::make_tuple(info.StrVal));
			DynamicCall<ICount - 1>::Invoke(className, t2, params);
		}
	}
};

template<>
struct DynamicCall<0>
{
	template<typename...TArgs>
	static void Invoke(std::string className, std::tuple<TArgs...> t1, std::list<DataInfo>& params)
	{
	}
};

int main()
{

	RegistToFactory<C1, std::string, std::string>();

	std::list<DataInfo> params;
	params.push_back(DataInfo("v1"));
	params.push_back(DataInfo("test c1"));

	DynamicCall<5>::Invoke(typeid(C1).name(), std::make_tuple(), params);


	RegistToFactory<C2, int, std::string>();
	params.clear();
	params.push_back(DataInfo(1));
	params.push_back(DataInfo("test c2"));

	DynamicCall<5>::Invoke(typeid(C2).name(), std::make_tuple(), params);

	return 0;
}
