#include <stdlib.h>

void f(void)
{
	int* x = (int*)malloc(10 * sizeof(int));
	x[10] = 0;  
}

int main(void)
{
	f();
	return 0;
}