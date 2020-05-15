#include	"libpop.h"
#include	"libpipe.h"

POPClass::POPClass(	const char hostname[], const char user[], const char passwd[], 
					FILE *stream ){

	struct	servent	*srv;
	char	buffer[4096];
	char	*ptr;

	// Inicializacion de miembro de la clase
	s=-1;
	nmsg=0;
	msgs=NULL;
	log_stream=stream;


	if ( !( srv = getservbyname( "pop3", "tcp" ) ) )
		sprintf( buffer, "%d", 110 );
	else sprintf( buffer, "%d", srv->s_port );
	if ( ( s=PipeSocketCreate( hostname, buffer ) ) < 0 )
		throw( "POPClass::POPClass Error en funcion 'PipeSocketCreate'" );

	// Presentacion
	readln( buffer, sizeof( buffer ) );
	chkln( buffer );

	// Usuario
	sprintf( buffer, "USER %s", user );
	writeln( buffer );
	readln( buffer, sizeof( buffer ) );
	chkln( buffer );

	// Password
	sprintf( buffer, "PASS %s", passwd );
	writeln( buffer );
	readln( buffer, sizeof( buffer ) );
	try{ chkln( buffer ); }
	catch( ... ){ quit(); throw( "POPClass::POPClass USER/PASS erroneo" ); }

	// LIST inicial
	writeln( "LIST" );
	readln( buffer, sizeof( buffer ) );
	chkln( buffer );
	while( 1 ){
		readln( buffer, sizeof( buffer ) );
		if ( !strcmp( buffer, "." ) ) break;
		if ( !( ptr=strchr( buffer, ' ' ) ) )
			throw( "POPClass::POPClass Error decodificando LIST" );
		if ( !( msgs=(long *)realloc( (void *)msgs, sizeof( long ) * (nmsg+1) ) ) )
			throw( "POPClass::POPClass Error en 'realloc'" );
		msgs[nmsg]=strtol( ptr+1, NULL, 10 );
		if ( msgs[nmsg] <= 0 )
			throw( "POPClass::POPClass Error decodificando LIST" );
		nmsg++;
	} // while

} // POPClass::POPClass


int	POPClass::readln( char buffer[], int sizeof_buffer ){

	int		n=0;
	char	caux[32];

	if ( s < 0 ) throw( "POPClass::readln socket no inicializado" );
	if ( !buffer ) throw( "POPClass::readln buffer incorrecto" );
	if ( sizeof_buffer <= 0 ) throw( "POPClass::readln sizeof_buffer incorrecto" );

	while(	( n < ( sizeof_buffer - 1 ) ) && 
			( read( s, buffer+n, 1 ) == 1 ) && 
			( ( *(buffer+n) ) != '\r' ) ) n++;

	if ( ( *(buffer+n) ) != '\r' ) 
		throw( "POPClass::readln Error en funcion 'read'" );

	// Lectura del '\n'
	read( s, caux, 1 );

	buffer[n] = '\0';
	if ( log_stream )
		fprintf( log_stream, "< %s\n", buffer );
	return( n );

} // POPClass::readln

void	POPClass::writeln( const char buffer[] ){

	if ( s < 0 ) throw( "POPClass::writeln socket no inicializado" );
	if ( !buffer ) throw( "POPClass::writeln buffer incorrecto" );

	if ( write( s, buffer, strlen( buffer ) ) != strlen( buffer ) )
		throw( "POPClass::writeln Error en funcion 'write'" );

	if ( write( s, "\r\n", 2 ) != 2 )
		throw( "POPClass::writeln Error en funcion 'write'" );

	if ( log_stream )
			fprintf( log_stream, "> %s\n", buffer );

} // POPClass::writeln

void	POPClass::quit( ){

	char	 buffer[1024];

	writeln( "QUIT" );
	readln( buffer, sizeof( buffer ) );

} // POPClass::quit

void	POPClass::chkln( const char buffer[] ){

	if (	( strlen( buffer ) < strlen( POP_OK ) ) || 
			( memcmp( buffer, POP_OK, strlen( POP_OK ) ) ) )
		throw( "POPClass::chkln Error en funcion 'OK'" );

} // POPClass::chkln

void	POPClass::retr( long msg, FILE *stream ){

	char	buffer[4096];
	long	nbytes=0;
	long	l;
	char	end_caux[3];

	if ( !msgs || ( msg <= 0 ) || ( msg > nmsg ) )
		throw( "POPClass::retr Argumento erroneo 'nmsg'" );
	if ( !stream ) throw( "POPClass::retr Argumento erroneo 'stream'" );
	if ( !msgs[ msg -1 ] ) return;

	sprintf( buffer, "RETR %ld", msg );
	writeln( buffer );
	readln( buffer, sizeof( buffer ) );
	chkln( buffer );

	while ( nbytes < msgs[ msg -1 ] ){
		if ( ( l=read( s, buffer, ( POP_MIN( sizeof( buffer ), msgs[ msg -1 ] - nbytes ) ) ) )
			< 0 ) throw( "POPClass::retr Error en funcion 'read'" );
		fwrite( buffer, l, 1, stream );
		nbytes+=l;
	} // while

	memset( end_caux, '\0', sizeof( end_caux ) );
	// Sigo leyendo hasta que encuentro el final, o sea ".\r\n"
	do{
		if ( end_caux[0] ) fwrite( end_caux, 1, 1, stream );
		end_caux[0]=end_caux[1]; end_caux[1]=end_caux[2];
		if ( read( s, end_caux+2, 1 ) != 1 )
			throw( "POPClass::retr Error en funcion 'read'" );
	} while( memcmp( end_caux, ".\r\n", sizeof( end_caux ) ) ); // do

} // POPClass::retr

void	POPClass::dele( long msg ){

	char	buffer[64];

	if ( !msgs || ( msg <= 0 ) || ( msg > nmsg ) )
		throw( "POPClass::retr Argumento erroneo 'nmsg'" );
	if ( !msgs[ msg -1 ] ) return;

	sprintf( buffer, "DELE %d", (int)msg );
	writeln( buffer );
	readln( buffer, sizeof( buffer ) );
	chkln( buffer );
	msgs[ msg -1 ]=0;

} // POPClass::dele
