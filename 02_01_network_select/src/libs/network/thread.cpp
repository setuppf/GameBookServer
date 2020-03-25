#include "thread.h"

bool Thread::Start( ) {
	_isRun = true;
	_thread = std::thread( [ this ]( ) {
		while ( _isRun ) {
			Update( );
		}
	} );

	return true;
}

void Thread::Stop( ) {
	if ( _isRun )
		_isRun = false;
}

void Thread::Dispose( ) {
	Stop( );

	if ( _thread.joinable( ) )
		_thread.join( );
}

void Thread::Update( ) { }
