#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"libpop.h"

int	main( int argc, char *argv[] ){

	int		c;
	char	caux[1024];
	FILE	*stream;
	char	*ptr;

	char	*host=NULL;
	char	*user=NULL;
	char	*passwd=NULL;
	char	*dir="./";
	short	remove_flag=0;
	FILE	*log_file=NULL;

	while ( ( c = getopt( argc, argv, "?tru:p:h:d:" ) ) != EOF ){
		switch( c ){
		case	'h':
			host=optarg;
			break;
		case	'u':
			user=optarg;
			break;
		case	'p':
			passwd=optarg;
			break;
		case	'd':
			dir=optarg;
			break;
		case	'r':
			remove_flag=1;
			break;
		case	't':
			log_file=stdout;
			break;
		case	'?':
		default:
			fprintf( stderr, "%s -h <hostname> -u <user> -p <passwd> [-t] [-r] [-d <dir>]\n", argv[0] );
			return(1);
			break;
		} // switch
	} // while
	if ( !host || !user || !passwd ){
		fprintf( stderr, "%s -h <hostname> -u <user> -p <passwd> [-t] [-r] [-d <dir>]\n", argv[0] );
		return(1);
	} // if

	try{
		POPClass	pop( host, user, passwd, log_file );
		long nmsg=pop.nmsgs_get();
		for ( long l=1; l<=nmsg; l++ ){
			if ( !(ptr=tempnam( dir, "" )) ){
				fprintf( stderr, "Error en funcion 'tempnam'\n" );
				return(1);
			} // if
			sprintf( caux, "%s.msg", ptr );
			if ( !(stream=fopen( caux, "w" )) ){
				fprintf( stderr, "Error en funcion 'fopen' abriendo el fichero '%s'\n", caux );
				return(1);
			} // if
			pop.retr( l, stream );
		} // for
		if ( remove_flag ){
			for ( long l=1; l<=nmsg; l++ )
				pop.dele( l );
		} // if
	} catch( const char msg[]){
		fprintf( stderr, "%s\n", msg );
		return(1);
	} // catch

	return(0);

} // main


