
#include "server.h"

#ifndef WIN32
#include <signal.h>
#else
#include <csignal>
#endif

bool isRun = true;
void stop( int signo ) {
	isRun = false;
}

int main( int argc, char *argv[] ) {
	signal( SIGINT, stop );

	Server server;
	if ( !server.Listen( "127.0.0.1", 2233 ) )
		return 1;

	while ( isRun ) {
		if ( !server.Update( ) )
			break;

		server.DataHandler( );
	}

	server.Dispose( );

	return 0;
}