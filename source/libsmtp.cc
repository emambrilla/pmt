#include	<ctype.h>
#include	"libsmtp.h"
#include	"libpipe.h"

SMTPClass::SMTPClass( const char hostname[], const char user[], const char passwd[], FILE *stream, char cr ){

	struct	servent	*srv;
	char	buffer[4096];

	// Inicializacion de miembro de la clase
	s=-1;
	log_stream=stream;
	smtp_cr=cr;

	if ( !( srv = getservbyname( "smtp", "tcp" ) ) )
		sprintf( buffer, "%d", 25 );
	else sprintf( buffer, "%d", srv->s_port );

	if ( ( s=PipeSocketCreate( hostname, buffer ) ) < 0 )
		throw( "SMTPClass::SMTPClass Error en funcion 'PipeSocketCreate'" );

	// Presentacion
	readln( buffer, sizeof( buffer ) );
	if ( lncode_get( buffer ) != 220 )
		throw( "SMTPClass::SMTPClass Error respuesta de conexion" );

	// Envio de HELO
	if ( passwd && strlen( passwd ) ){
		sprintf( buffer, "EHLO %s", user );
		writeln( buffer );
		for( ; ; ){
			readln( buffer, sizeof( buffer ) );
			if ( lncode_get( buffer ) != 250 )
				throw( "SMTPClass::send Error respuesta de 'EHLO'" );
			if ( buffer[3] == ' ' ) break;
			else if ( buffer[3] != '-' )
				throw( "SMTPClass::send Error respuesta de 'EHLO'" );
		} // for
		sprintf( buffer, "x%sx%s", user, passwd );
		buffer[0]=buffer[strlen(user)+1]='\0';
		sprintf( buffer, "AUTH PLAIN %s", SMTPEncode( buffer, strlen(user)+strlen(passwd)+2 ) );
		writeln( buffer );
		readln( buffer, sizeof( buffer ) );
		if ( lncode_get( buffer ) != 235 )
			throw( "SMTPClass::send Error respuesta de 'EHLO'" );
	} // if
	else{
		sprintf( buffer, "HELO %s", user );
		writeln( buffer );
		readln( buffer, sizeof( buffer ) );
		if ( lncode_get( buffer ) != 250 )
			throw( "SMTPClass::send Error respuesta de 'HELO'" );
	} // else

} // SMTPClass::SMTPClass


int	SMTPClass::readln( char buffer[], int sizeof_buffer ){

	int		n=0;
	char	caux[32];

	if ( s < 0 ) throw( "SMTPClass::readln socket no inicializado" );
	if ( !buffer ) throw( "SMTPClass::readln buffer incorrecto" );
	if ( sizeof_buffer <= 0 ) throw( "SMTPClass::readln sizeof_buffer incorrecto" );

	while(	( n < ( sizeof_buffer - 1 ) ) && 
			( read( s, buffer+n, 1 ) == 1 ) && 
			( ( *(buffer+n) ) != '\r' ) ) n++;

	if ( ( *(buffer+n) ) != '\r' ) 
		throw( "SMTPClass::readln Error en funcion 'read'" );

	// Lectura del '\n'
	read( s, caux, 1 );

	buffer[n] = '\0';
	if ( log_stream )
		fprintf( log_stream, "< %s\n", buffer );
	return( n );

} // SMTPClass::readln

void	SMTPClass::writeln( const char buffer[] ){

	if ( s < 0 ) throw( "SMTPClass::writeln socket no inicializado" );
	if ( !buffer ) throw( "SMTPClass::writeln buffer incorrecto" );

	if ( write( s, buffer, strlen( buffer ) ) != strlen( buffer ) )
		throw( "SMTPClass::writeln Error en funcion 'write'" );

	if ( write( s, "\r\n", 2 ) != 2 )
		throw( "SMTPClass::writeln Error en funcion 'write'" );

	if ( log_stream )
			fprintf( log_stream, "> %s\n", buffer );

} // SMTPClass::writeln

void	SMTPClass::quit( ){

	char	 buffer[1024];

	writeln( "QUIT" );
	readln( buffer, sizeof( buffer ) );

} // SMTPClass::quit

int	SMTPClass::lncode_get( const char buffer[] ){

	char	*ptr;
	int		n;

	n=(int)strtol( buffer, &ptr, 10 );
	if ( *ptr != ' ' && *ptr != '-' )
		throw( "SMTPClass::lncode_get Error en funcion 'write'" );

	return n;

} // SMTPClass::lncode_get

void	SMTPClass::send( const char from[], const char to[], const char subject[],  FILE *stream ){

	char	buffer[1096];
	char	myto[1096];
	long	l, j;

	// Envio de FROM
	sprintf( buffer, "MAIL FROM: <%s>", from );
	writeln( buffer );
	readln( buffer, sizeof( buffer ) );
	if ( lncode_get( buffer ) != 250 )
		throw( "SMTPClass::send Error respuesta de 'MAIL FROM'" );

	// Envio de TO
	if ( to && strlen( to ) ) strcpy( myto, to );
	else{
		// Obtengo to de la primera linea del fichero
		if ( !stream ) throw( "SMTPClass::send imposible obtener el to" );
		// Lectura de la primera linea
		if ( !fgets( buffer, sizeof( buffer ), stream ) )
			throw( "SMTPClass::send imposible obtener el to" );
		if ( strlen( buffer ) <= strlen ( SMTP_TO ) )
			throw( "SMTPClass::send imposible obtener el to" );
		if ( buffer[ strlen( buffer )-1 ] == '\n' ) buffer[ strlen( buffer )-1 ]='\0';
		if ( buffer[ strlen( buffer )-1 ] == '\r' ) buffer[ strlen( buffer )-1 ]='\0';
		strncpy( myto, buffer, strlen ( SMTP_TO ) );
		for( int i=0; i<strlen(myto); i++ ) myto[i]=toupper( myto[i] );
		if ( strcmp( myto, SMTP_TO ) ) throw( "SMTPClass::send imposible obtener el to" );
		strcpy( myto, buffer+strlen ( SMTP_TO ) );
	} // else
	sprintf( buffer, "RCPT TO: <%s>", myto );
	writeln( buffer );
	readln( buffer, sizeof( buffer ) );
	if ( lncode_get( buffer ) != 250 )
		throw( "SMTPClass::send Error respuesta de 'RCPT TO'" );

	if ( stream ){
		writeln( "DATA" );
		readln( buffer, sizeof( buffer ) );
		if ( lncode_get( buffer ) != 354 )
			throw( "SMTPClass::send Error en 'DATA'" );
		if ( to && strlen( to ) ){
			sprintf( buffer, "To: %s\n", myto );
			write( s, buffer, strlen( buffer ) );
		} // if
		if ( subject && strlen( subject ) ){
			sprintf( buffer, "Subject: %s\n", subject );
			write( s, buffer, strlen( buffer ) );
		} // if
		for( ; ; ){
			l=fread( buffer, 1, sizeof( buffer ), stream );
			if ( l > 0 ){
				if ( smtp_cr != '\0' ){
					for ( j=0; j < l; j++ ){
						if ( buffer[j] == smtp_cr ) buffer[j]='\n';
					} // for
				} // if
				write( s, buffer, l );
				if ( l < sizeof( buffer ) ) break;
			} // if
			else if ( l == 0 ) break;
			else throw( "SMTPClass::send Error en 'fread'" );
		} // while
		writeln( "" );
		writeln( "." );
		readln( buffer, sizeof( buffer ) );
		if ( lncode_get( buffer ) != 250 )
			throw( "SMTPClass::send Error en 'DATA'" );
	} // if

} // SMTPClass::send

char *SMTPEncode( char buffer[], int size_of_buffer ){

	static	char	*b64_code="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static	char	b64_encoded[4096];

	b64_encoded[0]='\0';
	memset( b64_encoded, '\0', sizeof( b64_encoded ) );
	for ( int i=0; i<size_of_buffer; i+=3 ){
		if ( ( size_of_buffer - i ) == 1 ){
			b64_encoded[strlen(b64_encoded)]=b64_code[buffer[i] >> 2];
			b64_encoded[strlen(b64_encoded)]=b64_code[(buffer[i] & (char)3) << 4 ];
			b64_encoded[strlen(b64_encoded)]='=';
			b64_encoded[strlen(b64_encoded)]='=';
			break;
		} // if
		else if ( ( size_of_buffer - i ) == 2 ){
			b64_encoded[strlen(b64_encoded)]=b64_code[buffer[i] >> 2];
			b64_encoded[strlen(b64_encoded)]=
				b64_code[((buffer[i] & (char)3) << 4 ) | (buffer[i+1] >> 4)];
			b64_encoded[strlen(b64_encoded)]=b64_code[(buffer[i+1] & (char)15) << 2 ];
			b64_encoded[strlen(b64_encoded)]='=';
			break;
		} // else if
		else{
			b64_encoded[strlen(b64_encoded)]=b64_code[buffer[i] >> 2];	
			b64_encoded[strlen(b64_encoded)]=
				b64_code[((buffer[i] & (char)3) << 4 ) | (buffer[i+1] >> 4)];
			b64_encoded[strlen(b64_encoded)]=b64_code[((buffer[i+1] & (char)15) << 2)|
				( buffer[i+2] >> 6 )  ];
			b64_encoded[strlen(b64_encoded)]=b64_code[buffer[i+2] & (char)63 ];
		} // else
	} // for

	return( b64_encoded );

} // SMTPEncode
