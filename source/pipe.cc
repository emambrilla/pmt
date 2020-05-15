#include	<stdio.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<signal.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	"libpipe.h"

int	pipe( int s );
char	*start_string=NULL;

int	main( int argc, char *argv[] ){

	struct  sockaddr_in	 myaddr_in;
	struct  sockaddr_in	 peeraddr_in;
	struct  servent *sp;
	int		ls;
	int		s;
	unsigned	short		port=0;
	char	*ptr;
	int		c;

	while ( ( c = getopt( argc, argv, "?p:s:" ) ) != EOF ){
		switch( c ){
		case	'p':
			port=(unsigned short)strtol( optarg, &ptr, 10 );
			if ( ( *ptr != '\0' ) || ( port <= 0 ) ){
				fprintf( stderr, "%s: puerto erroneo\n", argv[0] );
				goto return_error;
			} // if
			break;
		case	's':
			start_string=optarg;
			break;
		case	'?':
		default:
			fprintf( stderr, "%s -p <port> [-s <start_string>]\n", argv[0] );
			return(1);
			break;
		} // switch
	} // while
	if ( port <= 0 ){
		fprintf( stderr, "%s -p <port>\n", argv[0] );
		return(1);
	} // if

	memset( (void *)&myaddr_in, 0, sizeof( struct sockaddr_in ) );
	myaddr_in.sin_family = AF_INET;
#ifdef	_LINUX
	myaddr_in.sin_port = htons( port );
#else
	myaddr_in.sin_port = port;
#endif

	//	  Creacion del socket
	if ( ( ls = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ){
		fprintf( stderr, "%s(%s): Error creando socket\n", argv[0], "server" );
		goto return_error;
	} // if
	if ( bind( ls, (struct sockaddr *)&myaddr_in, sizeof( struct sockaddr_in ) ) == -1 ){
		fprintf( stderr, "%s(%s): Error creando bind\n", argv[0], "server" );
		goto return_error;
	} // if
	if ( listen( ls, 5 ) == -1 ){
		fprintf( stderr, "%s(%s): Error en funcion 'listen'\n", argv[0], "server" );
		goto return_error;
	} // if
	setpgrp();

	switch( fork() ){
	case	-1:			 //	  Error
		fprintf( stderr, "%s(%s): Error en funcion 'fork(1)'\n", argv[0], "server" );
		goto return_error;
		break;
	case	0:			  //	  Hijo
		printf( "kill %d\n", (int)getpid() );
		fclose( stdin );
		fclose( stderr );
		fclose( stdout );
#ifdef	_MAC
		signal( SIGCHLD, SIG_IGN );
#else
		signal( SIGCLD, SIG_IGN );
#endif
		for( ; ; ){
#ifdef	_LINUX
			socklen_t		addrlen;
#elif	_MAC
			socklen_t		addrlen;
#else
			int		addrlen;
#endif
			addrlen = sizeof( struct sockaddr_in );
			if ( (s=accept( ls, (struct sockaddr *)&peeraddr_in, &addrlen )) == -1 )
				goto return_error;
			switch( fork() ){
			case	-1:
				goto return_error;
				break;
			case	0:
				close( ls );
				pipe( s );
				break;
			default:
				close( s );
				break;
			} // switch
		} // for
		goto return_ok;
		break;
	default:
		goto return_ok;
	} // switch

return_ok:
	return(0);

return_error:
	return(1);

} // main

int	pipe( int s ){


	int		s_out=-1;
	char	buffer[4096];
	int		i;

	if ( start_string ){
		if ( ( s_out=PipeSocketCreate( start_string ) ) < 0 ) goto return_error;
	} // if
	else{
		// Leo y decodifico como primera linea <host>:<port>\n
		for( i=0; i < sizeof( buffer ); ){
			if ( read( s, buffer+i, 1 ) != 1 ) goto return_error;
			if ( buffer[i] == '\n' ){ buffer[i] = '\0'; break; }
			if ( buffer[i] != '\r' ) i++;
		} // for
		if ( i >= sizeof( buffer ) ) goto return_error;
		if ( ( s_out=PipeSocketCreate( buffer ) ) < 0 ) goto return_error;
	}
	if ( PipeExecute( s, s_out ) ) goto return_error;

return_ok:
	if ( s_out >= 0 ) close( s_out );
	return( 0 );

return_error:
	if ( s_out >= 0 ) close( s_out );
	return( -1 );

} // pipe
