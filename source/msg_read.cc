#include	<stdio.h>
#include	"libmsg.h"


int	main( int argc, char *argv[] ){

	try{
		char	*ptr;
		MSGClass	msg( "/users/emambr/pmt/msgs/BAAfCaaLU.msg" );
		ptr=msg.body_find( "DATE" );
		if ( ptr ) fprintf( stdout, "%s\n", ptr );
	} catch( const char merr[] ){
		fprintf( stderr, "%s: %s\n", argv[0], merr );
		goto return_error;
	} // catch

return_ok:
	return( 0 );

return_error:
	return( -1 );

} // main
