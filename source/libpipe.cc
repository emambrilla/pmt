#include	"libpipe.h"

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<string.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<poll.h>
#include	<stdio.h>
#include	<errno.h>

int	PipeSocketCreate( const char hostname[], const char portname[] ){

	struct	sockaddr_in	myaddr_in;
	struct	sockaddr_in	peeraddr_in;
	struct	hostent	*hp;
	struct	servent	*srv;
	char	buffer[4096];
	int		s=-1;
	char	*ptr;

	int		h_errnop;
	char	host[4096];
	char	*port;
	char	*next_address;

	strcpy( host, hostname );
	if ( portname ){ strcat( host, ":" ); strcat( host, portname ); }

	// Decodificacion de next address
	next_address=strchr( host, '/' );
	if ( next_address ){ *next_address='\0'; next_address++; }

	// Decodificacion de host y port
	port=strchr( host, ':' );
	if ( port ){ *port='\0'; port++; }

	// Apertura del socket saliente
	memset( (char *)&myaddr_in, 0, sizeof( struct sockaddr_in ) );
	memset( (char *)&peeraddr_in, 0, sizeof( struct sockaddr_in ) );
	peeraddr_in.sin_family = AF_INET;

	if ( !( hp = gethostbyname( host ) ) )
		goto return_error;
	peeraddr_in.sin_addr.s_addr=((struct in_addr *)(hp->h_addr))->s_addr;
	if ( !( srv = getservbyname( port, "tcp" ) ) ){
		unsigned short uiport= (unsigned short)strtol( port, &ptr, 10 );
		if ( ( *ptr != '\0' ) || ( uiport <= 0 ) ) goto return_error;
#ifdef	_LINUX
		peeraddr_in.sin_port=htons(uiport);
#else
		peeraddr_in.sin_port=uiport;
#endif
	} // if
	else peeraddr_in.sin_port=srv->s_port;

	if ( ( s=socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		goto return_error;

	if ( connect( s, (struct sockaddr *)&peeraddr_in, sizeof( struct sockaddr_in ) ) == -1 )
		goto return_error;

#ifdef	_LINUX
	socklen_t		addrlen;
#elif _MAC
	socklen_t		addrlen;
#else
	int		addrlen;
#endif
	addrlen = sizeof( struct sockaddr_in );
	if ( getsockname( s, (struct sockaddr *)&myaddr_in, &addrlen ) == -1 )
		goto return_error;

	if ( next_address && strlen( next_address ) ){
		write( s, next_address, strlen( next_address ) );
		write( s, "\n", 1 );
	} // if


	return( s );


return_error:
	if ( s >= 0 ) close( s );
	return( -1 );

} // PipeSocketCreate

int	PipeExecute( int s_in, int s_out ){


	struct	pollfd	fds[2];
	char	buffer[4096];
	int		i;

	fds[0].fd=s_in; fds[1].fd=s_out;
#ifdef	_LINUX
	fds[0].events=fds[1].events=POLLIN|POLLPRI|POLLHUP;
#else
	fds[0].events=fds[1].events=POLLIN|POLLRDNORM|POLLRDBAND|POLLPRI|POLLHUP;
#endif
	for( ; ; ){
		i=poll( fds, 2, -1 );
		if ( i > 0 ){
			if ( ( fds[0].revents & POLLIN ) ||
#ifdef	_LINUX
#else
				 ( fds[0].revents & POLLRDNORM ) ||
				 ( fds[0].revents & POLLRDBAND ) ||
#endif
				 ( fds[0].revents & POLLPRI ) ){
				// Ha llegado algo por in, escribo en out
				if ( read( s_in, buffer , 1 ) != 1 ) goto return_ok;
				if ( write( s_out, buffer , 1 ) != 1 ) goto return_ok;
			} // if
			else if ( fds[0].revents & POLLHUP ){
				// Ha cerrado in
				for ( ; ; ){
					if ( read( s_in, buffer , 1 ) != 1 ) break;
					if ( write( s_out, buffer , 1 ) != 1 ) goto return_ok;
				} // for
				goto return_ok;
			} // else if
			else if ( fds[0].revents & POLLERR ){
				// Error en in
				goto return_ok;
			} // else if
			if ( ( fds[1].revents & POLLIN ) ||
#ifdef	_LINUX
#else
				 ( fds[1].revents & POLLRDNORM ) ||
				 ( fds[1].revents & POLLRDBAND ) ||
#endif
				 ( fds[1].revents & POLLPRI ) ){
				// Ha llegado algo por out, escribo en in
				if ( read( s_out, buffer , 1 ) != 1 ) goto return_ok;
				if ( write( s_in, buffer , 1 ) != 1 ) goto return_ok;
			} // if
			else if ( fds[1].revents & POLLHUP ){
				// Ha cerrado out
				goto return_ok;
			} // else if
			else if ( fds[1].revents & POLLERR ){
				// Error en out
				goto return_ok;
			} // else if
		} // if
		else if ( i < 0 ){
			goto return_ok;
		} // if
	} // for

return_ok:
	return( 0 );

return_error:
	return( -1 );

} // PipeExecute
