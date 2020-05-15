#ifndef	libpop_h
#define	libpop_h

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>

#define	POP_MIN(a,b)	(a<=b)?(a):(b)
#define	POP_OK		"+OK"
#define	POP_ERR		"+ERR"

class	POPClass{

private:
	int		s;
	int		readln( char buffer[], int sizeof_buffer );
	void	writeln( const char buffer[] );
	void	quit();
	void	chkln( const char buffer[] );

	long	nmsg;
	long	*msgs;

	FILE	*log_stream;

public:
	POPClass( const char hostname[], const char user[], const char passwd[],
		FILE *stream=NULL );
	~POPClass(){ 
		try{ 
			if ( s >= 0 ) quit();
		} // try
		catch( ... ){ }
		if ( s >= 0 ) close( s ); 
		s=-1;
		if ( msgs ) free( msgs );
		msgs=NULL;
	} // ~POPClass
	long nmsgs_get() const{ return( nmsg ); } // nmsgs_get
	long msg_size_get( long nmsg ) const{ 
		if ( !msgs || ( nmsg <= 0 ) || ( nmsg > nmsg ) )
			throw( "POPClass::msg_size_get Argumento erroneo" );
		return( msgs[nmsg-1] ); 
	} // msg_size_get
	void	retr( long nmsg, FILE *stream );
	void	dele( long msg );

}; // POPClass

#endif // libpop_h
