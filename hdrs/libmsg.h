#ifndef	libmsg_h
#define	libmsg_h

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>

#define		MSG_ITEM_STRING	" = "

class	MSGInfoItemClass{
private:
	char	*label;
	char	*content;

public:
	MSGInfoItemClass( const char lb[], const char ct[] );
	~MSGInfoItemClass(){	if( label ) delete label;
							if( content ) delete content; }
	void	add( const char ct[] );
	void	pf( FILE *stream ){ 
					fprintf( stream , "\t%s: %s\n", label, content ); } // pf
	char	*label_get(){ return( label ); }
	char	*content_get(){ return( content ); }
}; // MSGInfoItemClass

class	MSGArrayClass{
private:
	int				nitems;
	int				nreserv_items;
	MSGInfoItemClass **info;

public:
	MSGArrayClass(){ info=NULL; nitems=nreserv_items=0; };
	~MSGArrayClass(){
		if ( info ){
			for( int i=0; i<nitems; i++ ) delete info[i];
			free(info);
		} // if
	} // ~MSGArrayClass
	void	add( const char lb[], const char ct[] );
	void	pf( FILE *stream ) const{ 
		for( int i=0; i<nitems; i++ ) info[i]->pf( stream ); } //pf
	char	*find( const char label[] );

}; // MSGArrayClass

class	MSGClass{

private:

	MSGArrayClass	head;
	MSGArrayClass	body;

	int	readln( FILE *stream, char buffer[], int sizeof_buffer );
	void msg_toupper( char string[] ){ 
		for( int i=0; i<strlen( string ); i++ ) string[i]=toupper(string[i]);
	} // toupper
	void	load( FILE *stream );

public:
	MSGClass( FILE *stream ){ load( stream ); };
	MSGClass( const char file[] );
	~MSGClass(){};
	void	pf( FILE *stream ) const{	fprintf( stdout, "Cabecera:\n" );
										head.pf( stream ); 
										fprintf( stdout, "Cuerpo:\n" );
										body.pf( stream ); } // pf
	char	*head_find( const char label[] ){ return( head.find( label ) ); }
	char	*body_find( const char label[] ){ return( body.find( label ) ); }

}; // MSGClass

#endif // libmsg_h
