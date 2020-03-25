// heap.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <cstdio>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>


#define HeapMin 1

void Show(std::vector<int>& data)
{
    std::cout << "数据列表：";
    for (int x : data)
        std::cout << x << " ";
    std::cout << "顶端：" << data.front() << std::endl;
    std::cout << std::endl << std::endl;
}

void PopData(std::vector<int>& data)
{
#ifdef HeapMin
    // 弹出heap顶元素, 将其放置于区间末尾
    pop_heap(data.begin(), data.end(), std::greater<int>());
#else
    pop_heap(data.begin(), data.end());
#endif

    std::cout << "弹出数据：" << data.back() << std::endl;
    data.pop_back();

    Show(data);
}

void PushData(std::vector<int>& data, const int value)
{
    std::cout << "往堆中添加元素：" << value << std::endl;
    data.push_back(value);

#ifdef HeapMin
    push_heap(data.begin(), data.end(), std::greater<int>());
#else
    push_heap(data.begin(), data.end());
#endif

    Show(data);
}

int main()
{
    std::vector<int> data{ 9, 1, 6, 3, 8, 9 };
    Show(data);

    std::cout << "执行make_heap." << std::endl;
#ifdef HeapMin
    make_heap(data.begin(), data.end(), std::greater<int>());
#else
    make_heap(data.begin(), data.end());
#endif

    Show(data);

    PopData(data);
    PopData(data);

    PushData(data, 5);
    PushData(data, 1);

    PopData(data);

    return 0;
}
