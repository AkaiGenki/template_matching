#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "pnmimg.h"

#define RBUFLEN		1026
static char readBuf[ RBUFLEN ] ;

#ifdef __STDC__
char *
nextWord( FILE *fp )
#else
char *
nextWord( fp )
     FILE *fp ;
#endif
{
  int idx ;
  int ch ;

  /* skip spaces */
  while( isspace( ch = getc( fp ))) {}

  /* push back one character, it is not a space */
  ungetc( ch, fp ) ;

  /* read a word into readBuf */
  for( idx = 0 ; idx < RBUFLEN - 1 ; idx++ ) {
    ch = getc( fp ) ;
    if ( ch == EOF ) {
      if ( idx ) { readBuf[idx] = NULL ; return( readBuf ) ; }
      return((char*)NULL) ;
    }
    if ( isspace( ch )) {
      readBuf[ idx ] = '\0' ;
      return( readBuf ) ;
    }
    else if ( ch == '#' ) {
      readBuf[0] = '#' ; readBuf[1] = '\0' ; return( readBuf ) ;
    }
    else {
      readBuf[ idx ] = (char) ch ;
    }
  }
  /* warning: buffer overflowed */
  readBuf[ RBUFLEN - 1 ] = '\0' ;
  return( readBuf ) ;
}

#ifdef __STDC__
char *
readPbmComment( FILE *fp )
#else
char *
readPbmComment( fp )
     FILE *fp ;
#endif
{
  int idx ;
  int ch ;

  /* read a line into readBuf */
  for( idx = 0 ; idx < RBUFLEN - 1 ; idx++ ) {
    ch = getc( fp ) ;
    if ( ch == EOF ) {
      readBuf[ idx ] = '\0' ; return( readBuf ) ;
    }
    if ( ch == '\n' ) {
      readBuf[ idx++ ] = '\n' ; readBuf[ idx ] = '\0' ; return( readBuf ) ;
    }
    readBuf[ idx ] = (char) ch ;
  }
  readBuf[ RBUFLEN - 1 ] = '\0' ;
  return( readBuf ) ;
}

#ifdef __STDC__
int
readPnmHeader( FILE *fp, int *cols, int *rows, int *maxval )
#else
int
readPnmHeader( fp, cols, rows, maxval )
     FILE *fp ;
     int *cols, *rows, *maxval ;
#endif
{
  char *key ;
  int  step ;
  int  encording ;
  int  type ;

  if ( !( key = nextWord( fp )))
    return( setError( E_FILE_EOF )) ;
  if ( strncmp( key, "P2", 2 ) == 0 ) {
    type = PGM ; encording = ASCII ; 
  } else if ( strncmp( key, "P5", 2 ) == 0 ) {
    type = PGM ; encording = BINARY ;
  } else if ( strncmp( key, "P3", 2 ) == 0 ) {
    type = PPM ; encording = ASCII ;
  } else if ( strncmp( key, "P6", 2 ) == 0 ) {
    type = PPM ; encording = BINARY ;
  } else if (( strncmp( key, "P1", 2 ) == 0 )
           ||( strncmp( key, "P4", 2 ) == 0 )) {
    return( setError( E_FILE_PBM )) ;
  } else {
    return( setError( E_FILE_NOTPNM )) ;
  }

  for ( step = 0 ; step < 3 ; ) {
    key = nextWord( fp ) ;
    if ( !key ) {
      return( setError( E_FILE_EOF )) ;
    }
    if ( *key == '#' ) { /* beginning of comment, read and discard it */
      readPbmComment( fp ) ;
    }
    else if ( isdigit( *key )) {
      switch( step ) {
        case 0 : sscanf( key, "%d", cols ) ; break ;
        case 1 : sscanf( key, "%d", rows ) ; break ;
        case 2 : sscanf( key, "%d", maxval ) ; break ;
      }
      step ++ ;
    }
    else {
      return( setError( E_FILE_FORM )) ;
    }
  }
  return( type | encording ) ;
}

#ifdef __STDC__
GRAY_IMAGE *
allocGrayImage( int cols, int rows )
#else
GRAY_IMAGE *
allocGrayImage( cols, rows )
     int cols ;
     int rows ;
#endif
{
  GRAY_IMAGE *img ;
  int i ;

  if (!( img = (GRAY_IMAGE*)malloc( sizeof(GRAY_IMAGE)))) {
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->p = (BYTE**)malloc( rows * sizeof(BYTE*)))) {
    free( img ) ;
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->data_p = (BYTE*)malloc( cols * rows * sizeof(BYTE)))) {
    free( img->p ) ;
    free( img ) ;
    setError( E_MALLOC ) ;
    return(0) ;
  }
  img->cols = cols ;
  img->rows = rows ;
  for ( i = 0 ; i < rows ; i++ ) {
    img->p[i] = &( img->data_p[ i * cols ] ) ;
  }
  return( img ) ;
}

#ifdef __STDC__
void
freeGrayImage( GRAY_IMAGE *img )
#else
void
freeGrayImage( img )
     GrayImage *img ;
#endif
{
  free( img->data_p ) ;
  free( img->p ) ;
  free( img ) ;
}

#ifdef __STDC__
GRAY_IMAGE *
readGrayImage( char *name )
#else
GRAY_IMAGE *
readGrayImage( name )
     char *name ;
#endif
{
  int rows ;
  int cols ;
  int maxval ;
  int size ;
  int ret ;
  int numio ;
  int nread ;
  int idx ;
  char *arg ;
  GRAY_IMAGE *img ;
  FILE *fp ;
  int ch ;

  if (!( fp = fopen( name, "rb" ))) {
    setError( E_FILE_OPEN ) ;
    printError( name ) ;
    return(0) ;
  }
  if (( ret = readPnmHeader( fp, &cols, &rows, &maxval )) == HAS_ERROR ) {
    printError( name ) ;
    fclose( fp ) ;
    return(0) ;
  }
  if (( ret & PGM ) != PGM ) {
    setError( E_FILE_FORM ) ;
    printError( name ) ;
    fclose( fp ) ;
    return(0) ;
  }
  if ( maxval < 16 || maxval >= 256 ) {
    setError( E_FILE_MAXVAL ) ;
    printError( name ) ;
    fclose( fp ) ;
    return(0) ;
  }
  if (!( img = allocGrayImage( cols, rows ))) {
    printError( name ) ;
    fclose( fp ) ;
    return(0) ;
  }
  size = rows * cols ;
  if ( ret & BINARY ) {
    /*
     * input BINARY encorded PGM image
     */
    for( numio = 0 ; numio < size ; numio += nread ){
      nread = fread( &img->data_p[numio], 1, size - numio, fp ) ;
      if ( nread <= 0 ) {
	setError( E_FILE_READ ) ;
	printError( name ) ;
	freeGrayImage( img ) ;
	fclose( fp ) ;
	return(0) ;
      }
    }
  }
  else {
    /*
     * input ASCII encorded PGM image
     */
    for ( idx = 0 ; idx < size ; ) {
      if (( arg = nextWord( fp )) == (char*)NULL ) {
	setError( E_FILE_EOF ) ;
      }
      else if ( ! isdigit( *arg )) {
        if ( *arg == '#' ) { /* skip over comment line */
          while(( ch = getc( fp )) != '\n' && ch != EOF )
	    ;
        }
	else {	/* data must be digit numbers. can't happen */
	  setError( E_FILE_INVALID ) ;
	}
      }
      else
	img->data_p[idx++] = atoi( arg ) ;
      if ( checkError()) {
	printError( name ) ;
	freeGrayImage( img ) ;
	fclose( fp ) ;
	return(0) ;
      }
    }
  }
  fclose( fp ) ;
  return( img ) ;
}

#ifdef __STDC__
int
writeGrayImage( GRAY_IMAGE *img, char *name )
#else
int
writeGrayImage( img, name )
     GRAY_IMAGE *img ;
     char *name ;
#endif
{
  FILE *fp ;
  int idx ;
  BYTE *ptr ;

  if (!( fp = fopen( name, "wb" ))) {
    setError( E_FILE_OPEN ) ;
    printError( name ) ;
    return( HAS_ERROR ) ;
  }

  fprintf( fp, "P5\n%d %d\n255\n", img->cols, img->rows ) ;
  ptr = img->data_p ;
  for ( idx = 0 ; idx < img->rows ; idx++, ptr+=img->cols ) {
    if ( fwrite( ptr, 1, img->cols, fp ) != img->cols ) {
      setError( E_FILE_WRITE ) ;
      fclose( fp ) ;
      return( HAS_ERROR ) ;
    }
  }
  fclose( fp ) ;
  return( NO_ERROR ) ;
}

#ifdef __STDC__
GRAY_LONG_IMAGE *
allocGrayLongImage( int cols, int rows )
#else
GRAY_LONG_IMAGE *
allocGrayLongImage( cols, rows )
     int cols ;
     int rows ;
#endif
{
  GRAY_LONG_IMAGE *img ;
  int i ;

  if (!( img = (GRAY_LONG_IMAGE*)malloc( sizeof(GRAY_LONG_IMAGE)))) {
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->p = (long**)malloc( rows * sizeof(long*)))) {
    free( img ) ;
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->data_p = (long*)malloc( cols * rows * sizeof(long)))) {
    free( img->p ) ;
    free( img ) ;
    setError( E_MALLOC ) ;
    return(0) ;
  }
  img->cols = cols ;
  img->rows = rows ;
  for ( i = 0 ; i < rows ; i++ ) {
    img->p[i] = &( img->data_p[ i * cols ] ) ;
  }
  return( img ) ;
}

#ifdef __STDC__
void
freeGrayLongImage( GRAY_LONG_IMAGE *img )
#else
void
freeGrayLongImage( img )
     GRAY_LONG_IMAGE *img ;
#endif
{
  free( img->data_p ) ;
  free( img->p ) ;
  free( img ) ;
}

#ifdef __STDC__
GRAY_DOUBLE_IMAGE *
allocGrayDoubleImage( int cols, int rows )
#else
GRAY_DOUBLE_IMAGE *
allocGrayDoubleImage( cols, rows )
     int cols ;
     int rows ;
#endif
{
  GRAY_DOUBLE_IMAGE *img ;
  int i ;

  if (!( img = (GRAY_DOUBLE_IMAGE*)malloc( sizeof(GRAY_DOUBLE_IMAGE)))) {
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->p = (double**)malloc( rows * sizeof(double*)))) {
    free( img ) ;
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->data_p = (double*)malloc( cols * rows * sizeof(double)))) {
    free( img->p ) ;
    free( img ) ;
    setError( E_MALLOC ) ;
    return(0) ;
  }
  img->cols = cols ;
  img->rows = rows ;
  for ( i = 0 ; i < rows ; i++ ) {
    img->p[i] = &( img->data_p[ i * cols ] ) ;
  }
  return( img ) ;
}

#ifdef __STDC__
void
freeGrayDoubleImage( GRAY_DOUBLE_IMAGE *img )
#else
void
freeGrayDoubleImage( img )
     GRAY_DOUBLE_IMAGE *img ;
#endif
{
  free( img->data_p ) ;
  free( img->p ) ;
  free( img ) ;
}

#ifdef __STDC__
RGB_PACKED_IMAGE *
allocRGBPackedImage( int cols, int rows )
#else
RGB_PACKED_IMAGE *
allocRGBPackedImage( cols, rows )
     int cols ;
     int rows ;
#endif
{
  RGB_PACKED_IMAGE *img ;
  int i ;

  if (!( img = (RGB_PACKED_IMAGE*)malloc( sizeof(RGB_PACKED_IMAGE)))) {
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->p = 
	(RGB_PACKED_PIXEL**)malloc( rows * sizeof(RGB_PACKED_PIXEL*)))) {
    setError( E_MALLOC ) ;
    free( img ) ;
    return(0) ;
  }
  if (!( img->data_p = 
	(RGB_PACKED_PIXEL*)malloc( cols * rows * sizeof(RGB_PACKED_PIXEL)))) {
    setError( E_MALLOC ) ;
    free( img->p ) ;
    free( img ) ;
    return(0) ;
  }
  img->cols = cols ;
  img->rows = rows ;
  for ( i = 0 ; i < rows ; i++ ) {
    img->p[i] = &( img->data_p[ i * cols ] ) ;
  }
  return( img ) ;
}

#ifdef __STDC__
void
freeRGBPackedImage( RGB_PACKED_IMAGE *img )
#else
void
freeRGBPackedImage( img )
     RGB_PACKED_IMAGE *img ;
#endif
{
  free( img->data_p ) ;
  free( img->p ) ;
  free( img ) ;
}

#ifdef __STDC__
RGB_PACKED_IMAGE *
readRGBPackedImage( char *name )
#else
RGB_PACKED_IMAGE *
readRGBPackedImage( name )
     char *name ;
#endif
{
  int rows ;
  int cols ;
  int maxval ;
  int size ;
  int ret ;
  int numio ;
  int nread ;
  int idx ;
  char *arg ;
  RGB_PACKED_IMAGE *img ;
  FILE *fp ;
  int ch ;

  if (!( fp = fopen( name, "rw" ))) {
    setError( E_FILE_OPEN ) ;
    printError( name ) ;
    return(0) ;
  }
  if (( ret = readPnmHeader( fp, &cols, &rows, &maxval )) == HAS_ERROR ) {
    printError( name ) ;
    fclose( fp ) ;
    return(0) ;
  }
  if (( ret & PPM ) != PPM ) {
    setError( E_FILE_FORM ) ;
    printError( name ) ;
    fclose( fp ) ;
    return(0) ;
  }
  if ( maxval < 16 || maxval >= 256 ) {
    setError( E_FILE_MAXVAL ) ;
    printError( name ) ;
    fclose( fp ) ;
  }
  if (!( img = allocRGBPackedImage( cols, rows ))) {
    printError( name ) ;
    fclose( fp ) ;
  }
  size = rows * cols ;
  if ( ret & BINARY ) {
    /*
     * input BINARY encorded PPM image
     */
    for( numio = 0 ; numio < size ; numio += nread ){
      nread = fread( &img->data_p[numio], sizeof( RGB_PACKED_PIXEL), 
		     size - numio, fp ) ;
      if ( nread <= 0 ) {
	setError( E_FILE_READ ) ;
	printError( name ) ;
	freeRGBPackedImage( img ) ;
	fclose( fp ) ;
	return(0) ;
      }
    }

  }
  else {
    /*
     * input ASCII encorded PPM image
     */
    for ( idx = 0 ; idx < size ; idx++ ) {
      if (( arg = nextWord( fp )) == (char*)NULL ) /* read R value */
	setError( E_FILE_EOF ) ;
      else if ( ! isdigit( *arg )) {
        if ( *arg == '#' ) { /* skip over comment line */
          while(( ch = getc( fp )) != '\n' && ch != EOF ) {}
        }
	else 
	  setError( E_FILE_INVALID ) ;
      }
      else
	img->data_p[idx].r = atoi( arg ) ;
      if (( arg = nextWord( fp )) == (char*)NULL ) /* read G value */
	setError( E_FILE_EOF ) ;
      else if ( ! isdigit( *arg )) {
        if ( *arg == '#' ) { /* skip over comment line */
          while(( ch = getc( fp )) != '\n' && ch != EOF ) {}
        }
	else 
	  setError( E_FILE_INVALID ) ;
      }
      else
	img->data_p[idx].g = atoi( arg ) ;
      if (( arg = nextWord( fp )) == (char*)NULL ) /* read B value */
	setError( E_FILE_EOF ) ;
      else if ( ! isdigit( *arg )) {
        if ( *arg == '#' ) { /* skip over comment line */
          while(( ch = getc( fp )) != '\n' && ch != EOF ) {}
        }
	else 
	  setError( E_FILE_INVALID ) ;
      }
      else
	img->data_p[idx].b = atoi( arg ) ;
      if ( checkError()) {
	printError( name ) ;
	freeRGBPackedImage( img ) ;
	fclose( fp ) ;
	return(0) ;
      }
    }
  }
  fclose( fp ) ;
  return( img ) ;
}

#ifdef __STDC__
int
writeRGBPackedImage( RGB_PACKED_IMAGE *img, char *name )
#else
int
writeRGBPackedImage( img, name )
     RGB_PACKED_IMAGE *img ;
     char *name ;
#endif
{
  FILE *fp ;
  int size ;
  int idx ;
  RGB_PACKED_PIXEL *ptr ;

  if (!( fp = fopen( name, "wb" ))) {
    setError( E_FILE_OPEN ) ;
    printError( name ) ;
    return( HAS_ERROR ) ;
  }

  fprintf( fp, "P6\n%d %d\n255\n", img->cols, img->rows ) ;
  ptr = img->data_p ;
  size = img->cols ;
  for ( idx = 0 ; idx < img->rows ; idx++, ptr+=img->cols ) {
    if ( fwrite( ptr, sizeof(RGB_PACKED_PIXEL), size, fp ) != size ) {
      setError( E_FILE_WRITE ) ;
      printError( name ) ;
      fclose( fp ) ;
      return( HAS_ERROR ) ;
    }
  }
  fclose( fp ) ;
  return( NO_ERROR ) ;
}

#ifdef __STDC__
RGB_PLANAR_IMAGE *
allocRGBPlanarImage( int cols, int rows )
#else
RGB_PLANAR_IMAGE *
allocRGBPlanarImage( cols, rows )
     int cols ;
     int rows ;
#endif
{
  RGB_PLANAR_IMAGE *img ;
  int i ;

  clearError() ;
  if (!( img = (RGB_PLANAR_IMAGE*)calloc( 1, sizeof(RGB_PLANAR_IMAGE)))) {
    setError( E_MALLOC ) ;
    return(0) ;
  }
  if (!( img->r = (BYTE**)malloc( rows * sizeof(BYTE*))))
    setError( E_MALLOC ) ;
  if (!( img->g = (BYTE**)malloc( rows * sizeof(BYTE*))))
    setError( E_MALLOC ) ;
  if (!( img->b = (BYTE**)malloc( rows * sizeof(BYTE*))))
    setError( E_MALLOC ) ;
  if (!( img->data_r = (BYTE*)malloc( cols * rows * sizeof(BYTE))))
    setError( E_MALLOC ) ;
  if (!( img->data_g = (BYTE*)malloc( cols * rows * sizeof(BYTE))))
    setError( E_MALLOC ) ;
  if (!( img->data_b = (BYTE*)malloc( cols * rows * sizeof(BYTE))))
    setError( E_MALLOC ) ;
  if ( checkError()) {
    if ( img->data_g ) free( img->data_g ) ;
    if ( img->data_r ) free( img->data_r ) ;
    if ( img->b ) free( img->b ) ;
    if ( img->g ) free( img->g ) ;
    if ( img->r ) free( img->r ) ;
    free( img ) ;
    return(0) ;
  }
  img->cols = cols ;
  img->rows = rows ;
  for ( i = 0 ; i < rows ; i++ ) {
    img->r[i] = &( img->data_r[ i * cols ] ) ;
    img->g[i] = &( img->data_g[ i * cols ] ) ;
    img->g[i] = &( img->data_b[ i * cols ] ) ;
  }
  return( img ) ;
}

#ifdef __STDC__
void
freeRGBPlanarImage( RGB_PLANAR_IMAGE *img )
#else
void
freeRGBPlanarImage( img )
     RGB_PLANAR_IMAGE *img ;
#endif
{
  free( img->data_b ) ;
  free( img->data_g ) ;
  free( img->data_r ) ;
  free( img->b ) ;
  free( img->g ) ;
  free( img->r ) ;
  free( img ) ;
}

