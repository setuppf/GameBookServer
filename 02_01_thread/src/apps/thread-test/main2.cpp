#include <thread>
#include <iostream>
#include <mutex>

int global_value;
std::mutex mutex_obj;
void ThreadTest( ) {
	//std::lock_guard<std::mutex> guard( mutex_obj );

	global_value++;
	std::cout << "\nthread test. global_value:" << global_value << std::endl;
}

int main2( int argc, char *argv[] ) {
	global_value = 0;
	std::thread t1( ThreadTest );
	std::thread t2( ThreadTest );
	t1.join( );
	t2.join( );

	std::cout << "exit main" << std::endl;
	return 0;
}