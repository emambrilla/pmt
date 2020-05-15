#ifndef	libsmtp_h
#define	libsmtp_h

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>

#define	SMTP_TO	"TO: "

class	SMTPClass{

private:
	int	s;
	int	readln( char buffer[], int sizeof_buffer );
	void	writeln( const char buffer[] );
	int	lncode_get( const char buffer[] );
	void	quit();
	char	smtp_cr;

	FILE	*log_stream;

public:
	SMTPClass( const char hostname[], const char user[], const char passwd[]=NULL, 
		FILE *stream=NULL, char cr='\0' );
	~SMTPClass(){ 
		try{ 
			if ( s >= 0 ) quit();
		} // try
		catch( ... ){ }
		if ( s >= 0 ) close( s ); 
		s=-1;
	} // ~SMTPClass
	void	send( const char from[], const char to[], const char subject[],  FILE *stream );

}; // SMTPClass

char *SMTPEncode( char buffer[], int size_of_buffer );

#endif // libsmtp_h
