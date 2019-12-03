#include <stdio.h>

void fun2()
{
	for (int i = 0; i < 10000000; i++);
}

void fun1()
{

	for (int i = 0; i < 10000000; i++);

	fun2();
}

int main(void)
{
	fun1();
	return 0;
}