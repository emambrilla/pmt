#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"libsmtp.h"

int	main( int argc, char *argv[] ){

	int	c;
	char	caux[1024];
	char	*ptr;
	char	cr='\0';

	char	*host=NULL;
	char	*from=NULL;
	char	*user=NULL;
	char	*passwd=NULL;
	char	*file=NULL;
	char	*to=NULL;
	char	*subject=NULL;
	FILE	*log_file=NULL;
	FILE	*stream=NULL;

	while ( ( c = getopt( argc, argv, "?th:f:p:x:u:d:s:n:" ) ) != EOF ){
		switch( c ){
		case	'h':
			host=optarg;
			break;
		case	'p':
			passwd=optarg;
			break;
		case	'f':
			from=optarg;
			break;
		case	'u':
			user=optarg;
			break;
		case	'x':
			to=optarg;
			break;
		case	'd':
			file=optarg;
			break;
		case	's':
			subject=optarg;
			break;
		case	't':
			log_file=stdout;
			break;
		case	'n':
			cr=optarg[0];
			break;
		case	'?':
		default:
			fprintf( stderr, "%s -h <hostname> -u <user> -f <from> [-p <passwd>] [-s <subject>] [-d <file>] [-x <to>] [-t]\n", argv[0] );
			return(1);
			break;
		} // switch
	} // while
	if ( !host || !user || !from ){
		fprintf( stderr, "%s -h <hostname> -u <user> -f <from> [-p <passwd>] [-s <subject>] [-x <to>] [-d <file>] [-t]\n", argv[0] );
		return(1);
	} // if

	try{
		SMTPClass smtp( host, user, passwd, log_file, cr );
		if ( !file ) smtp.send( from, to, subject, stdin );
		else{
			if ( !( stream=fopen( file, "r" ) ) )
				throw( "msg_send Error al abrir fichero" );
			smtp.send( from, to, subject, stream );
		} // else
	} catch( const char msg[]){
		if ( stream ) fclose( stream );
		fprintf( stderr, "%s\n", msg );
		return(1);
	} // catch

	if ( stream ) fclose( stream );
	return(0);

} // main


