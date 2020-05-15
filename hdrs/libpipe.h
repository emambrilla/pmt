#ifndef	libpipe_h
#define	libpipe_h

#include	<string.h>

int	PipeSocketCreate( const char hostname[], const char portname[]=NULL );
int	PipeExecute( int s_in, int s_out );

#endif // libpipe_h
