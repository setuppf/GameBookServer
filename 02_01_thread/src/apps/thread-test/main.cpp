#include <thread>
#include <iostream>

void ThreadTest1( int value ) {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	std::cout << "thread test. value:" << value << std::endl;
}

int main( int argc, char *argv[] ) {
	int value = 1;
	std::thread t1( ThreadTest1, value );

	//t1.join( );
	t1.detach();

	std::cout << "exit main" << std::endl;
	return 0;
}