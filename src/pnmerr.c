#include <stdlib.h>
#include <stdio.h>
#include "pnmimg.h"

static int lastErrorType = E_NULL ;
typedef struct error_list {
    char *description ;
    int   issystem ;
} ERROR_LIST ;

static ERROR_LIST errList[MAX_ERRORS + 1] = {
    { "no error.", 0 },
    { "malloc failed.", 1 },
    { "file can't open.", 1 },
    { "error in reading from the file.", 0 },
    { "error in writing to the file.", 0 },
    { "not a pbm/pgm/ppm file.", 0 },
    { "invalid file format.", 0 },
    { "EOF detected while reading.", 0 },
    { "invalid data in the file.", 0 },
    { "file data maxval should be between 16 and 255.", 0 },
    { "not supported PBM format.", 0 }
} ;


#ifdef __STDC__
int
setError( int errtype )
#else
int
setError( errtype )
int errtype ;
#endif
{
    if ( errtype < 0 || errtype > MAX_ERRORS )
        errtype = E_UNKNOWN ;
    lastErrorType = errtype ;
    return ( HAS_ERROR ) ;
}

#ifdef __STDC__
void
clearError( void )
#else
void
setError()
#endif
{
    lastErrorType = E_NULL ;
}

#ifdef __STDC__
int
checkError( void )
#else
int
checkError()
#endif
{
    return ( lastErrorType ) ;
}

#ifdef __STDC__
void
printError( char *cause )
#else
void
printError( cause )
char *cause ;
#endif
{
#if defined(unix) || defined(MSDOS)
    if ( errList[ lastErrorType ].issystem ) {
        fprintf( stderr, "%s: %s: ",
                 cause, errList[ lastErrorType ].description ) ;
        perror((char*)NULL) ;
    }
    else {
        fprintf( stderr, "%s: %s\n",
                 cause, errList[ lastErrorType ].description ) ;
    }
#else
#endif
}

