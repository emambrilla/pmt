#include	"libmsg.h"
#include	<string.h>
#include	<stdlib.h>

MSGInfoItemClass::MSGInfoItemClass( const char lb[], const char ct[] ){

	if ( !lb || !ct ) 
		throw( "MSGInfoItemClass::MSGInfoItemClass Argumento erroneo" );
	label=strdup( lb );
	content=strdup( ct );
	
} // MSGInfoItemClass::MSGInfoItemClass

void	MSGInfoItemClass::add( const char ct[] ){

	char	*ptr=(char *)malloc( strlen( content ) + 1 + strlen( ct ) + 1 );

	strcpy( ptr, content ); strcat( ptr, " " ); strcat( ptr, ct );
	free( content ); content=ptr;

} // MSGInfoItemClass::add

void	MSGArrayClass::add( const char lb[], const char ct[] ){

	if ( lb ){
		if ( !info ){
			nitems=0;
			nreserv_items=5;
			info=(MSGInfoItemClass **)malloc( sizeof( MSGInfoItemClass * ) * 
												nreserv_items );
			if ( !info ) throw( "MSGArrayClass::add Error en 'malloc'" );
		} // if
		else if ( nitems >= nreserv_items ){
			nreserv_items*=2;
			info=(MSGInfoItemClass **)realloc( info,  
								sizeof( MSGInfoItemClass * ) * nreserv_items );
			if ( !info ) throw( "MSGArrayClass::add Error en 'realloc'" );
		} // else if
		info[nitems]=new MSGInfoItemClass( lb, ct );
		nitems++;
	} // if
	else{ if ( nitems > 0 ) info[nitems-1]->add( ct ); } //else

} // MSGArrayClass:add

MSGClass::MSGClass( const char file[] ){
	
	FILE	*stream;

	if ( !(stream=fopen( file, "r" ) ) )
			throw( "MSGClass::MSGClass Abriendo fichero" );

	try{
		load( stream );
	} catch( const char msg[] ){ fclose( stream ); throw msg; }

	fclose( stream );


} // MSGClass::MSGClass

void	MSGClass::load( FILE *stream ){

	char	buffer[4096];
	char	*ptr;

	// Lectura de la zona de cabecera
	for( ; ; ){
		if ( readln( stream, buffer, sizeof( buffer ) ) < 0 )
			throw( "MSGClass::MSGClass Fin de lectura en cabecera" );
		if ( !strlen( buffer ) ) break;	// Fin de cabecera
		if ( buffer[0] == ' ' ){
			// Prolongacion de la linea anterior
			for ( ptr=buffer; *ptr == ' '; ptr++ );
			head.add( NULL, ptr );
		} // if 
		else{
			// Nueva linea
			if ( !( ptr=strchr( buffer, ':' ) ) )
				throw( "MSGClass::MSGClass Linea de cabecera erronea(1)" );
			*ptr='\0'; ptr++;
			if ( !strlen( ptr ) ) 
				throw( "MSGClass::MSGClass Linea de cabecera erronea(2)" );
			ptr++;
			msg_toupper( buffer );
			head.add( buffer, ptr );
		} // else
	} // for

	// Lectura de la zona de cuerpo
	for( ; ; ){
		if ( readln( stream, buffer, sizeof( buffer ) ) < 0 ) break;
		if ( !strlen( buffer ) ) continue;
		if ( !( ptr=strstr( buffer, MSG_ITEM_STRING ) ) ) continue;
		*ptr='\0'; ptr+=strlen( MSG_ITEM_STRING );
		if ( !strlen( ptr ) ) continue;
		msg_toupper( buffer );
		if ( !strcmp( buffer, "DATE" ) ){

			char	*ptr_tok, *chk;
			int		ano, mes, dia, hora, minuto, segundo;

			ptr_tok=strtok( ptr, " " );
			if ( !ptr_tok ) 
				throw( "MSGClass::MSGClass Decodificando DATE(1)" );
			dia=(int)strtol( ptr_tok, &chk, 10 );
			if ( *chk != '\0' )
				throw( "MSGClass::MSGClass Decodificando DATE(2)" );

			ptr_tok=strtok( NULL, " " );
			if ( !ptr_tok ) 
				throw( "MSGClass::MSGClass Decodificando DATE(3)" );
			if ( !strcmp( ptr_tok, "Jan" ) ) mes=1;
			else if ( !strcmp( ptr_tok, "Feb" ) ) mes=2;
			else if ( !strcmp( ptr_tok, "Mar" ) ) mes=3;
			else if ( !strcmp( ptr_tok, "Apr" ) ) mes=4;
			else if ( !strcmp( ptr_tok, "May" ) ) mes=5;
			else if ( !strcmp( ptr_tok, "Jun" ) ) mes=6;
			else if ( !strcmp( ptr_tok, "Jul" ) ) mes=7;
			else if ( !strcmp( ptr_tok, "Aug" ) ) mes=8;
			else if ( !strcmp( ptr_tok, "Sep" ) ) mes=9;
			else if ( !strcmp( ptr_tok, "Oct" ) ) mes=10;
			else if ( !strcmp( ptr_tok, "Nov" ) ) mes=11;
			else if ( !strcmp( ptr_tok, "Dec" ) ) mes=12;
			else throw( "MSGClass::MSGClass Decodificando DATE(4)" );

			ptr_tok=strtok( NULL, " " );
			if ( !ptr_tok ) 
				throw( "MSGClass::MSGClass Decodificando DATE(5)" );
			ano=(int)strtol( ptr_tok, &chk, 10 );
			if ( *chk != '\0' )
				throw( "MSGClass::MSGClass Decodificando DATE(6)" );
			ano+=2000;

			ptr_tok=strtok( NULL, ":" );
			if ( !ptr_tok ) 
				throw( "MSGClass::MSGClass Decodificando DATE(7)" );
			hora=(int)strtol( ptr_tok, &chk, 10 );
			if ( *chk != '\0' )
				throw( "MSGClass::MSGClass Decodificando DATE(8)" );

			ptr_tok=strtok( NULL, ":" );
			if ( !ptr_tok ) 
				throw( "MSGClass::MSGClass Decodificando DATE(9)" );
			minuto=(int)strtol( ptr_tok, &chk, 10 );
			if ( *chk != '\0' )
				throw( "MSGClass::MSGClass Decodificando DATE(10)" );

			ptr_tok=strtok( NULL, " " );
			if ( !ptr_tok ) 
				throw( "MSGClass::MSGClass Decodificando DATE(9)" );
				segundo=(int)strtol( ptr_tok, &chk, 10 );
			if ( *chk != '\0' )
				throw( "MSGClass::MSGClass Decodificando DATE(11)" );

			sprintf( ptr, "%04d%02d%02d%02d%02d%02d",
						ano, mes, dia, hora, minuto, segundo );
		} // if
		body.add( buffer, ptr );
	} // for

} // MSGClass::load

int	MSGClass::readln( FILE *stream, char buffer[], int sizeof_buffer ){

	if ( !fgets( buffer, sizeof_buffer, stream ) ) return( -1 );
	if ( buffer[strlen(buffer)-1] == '\n' ) buffer[strlen(buffer)-1]='\0';
	if ( buffer[strlen(buffer)-1] == '\r' ) buffer[strlen(buffer)-1]='\0';

	return( strlen( buffer ) );

} // MSGClass::readln

char	*MSGArrayClass::find( const char label[] ){

	for ( int i=0; i<nitems; i++ ){
		if ( !strcmp( label, info[i]->label_get() ) )
				return( info[i]->content_get() );
	} // for
	return( NULL );

} // find
