typedef unsigned long  DWORD ;
typedef unsigned short WORD ;
typedef unsigned char  BYTE ;

typedef struct gray_image {
    int     cols ;
    int     rows ;
    BYTE  **p ;
    BYTE   *data_p ;
} GRAY_IMAGE ;

typedef struct gray_long_image {
    int     cols ;
    int     rows ;
    long  **p ;
    long   *data_p ;
} GRAY_LONG_IMAGE ;

typedef struct gray_double_image {
    int      cols ;
    int      rows ;
    double **p ;
    double  *data_p ;
} GRAY_DOUBLE_IMAGE ;

typedef struct rgb_packed_pixel {
    BYTE r ;
    BYTE g ;
    BYTE b ;
} RGB_PACKED_PIXEL ;

typedef struct rgb_packed_image {
    int                cols ;
    int                rows ;
    RGB_PACKED_PIXEL **p ;
    RGB_PACKED_PIXEL  *data_p ;
} RGB_PACKED_IMAGE ;

typedef struct rgb_planar_image {
    int     cols ;
    int     rows ;
    BYTE  **r ;
    BYTE  **g ;
    BYTE  **b ;
    BYTE   *data_r ;
    BYTE   *data_g ;
    BYTE   *data_b ;
} RGB_PLANAR_IMAGE ;

/*
 *  define type of PBM/PGM/PPM file format
 */
#define PGM 2
#define PPM 4
#define BINARY  8
#define ASCII   0

/*
 *  define library error code
 */
#define NO_ERROR    0
#define HAS_ERROR   -1

#define MAX_ERRORS  11
#define E_NULL      0
#define E_UNKNOWN   1
#define E_MALLOC    2
#define E_FILE_OPEN 3
#define E_FILE_READ 4
#define E_FILE_WRITE    5
#define E_FILE_NOTPNM   6
#define E_FILE_FORM 7
#define E_FILE_EOF  8
#define E_FILE_INVALID  9
#define E_FILE_MAXVAL   10
#define E_FILE_PBM  11

/*
 *  library function prototypes (if not in ANSI mode, don't protize arguments)
 */
#ifndef __P
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos) protos
#else
#define __P(protos) ()
#endif
#endif /* defined __P */

char            *nextWord __P(( FILE* ));
char            *readPbmComment __P(( FILE* ));
int         readPnmHeader __P(( FILE*, int*, int*, int* ));

GRAY_IMAGE      *allocGrayImage __P(( int, int ));
void            freeGrayImage __P(( GRAY_IMAGE* ));
GRAY_IMAGE      *readGrayImage __P(( char* ));
int         writeGrayImage __P(( GRAY_IMAGE*, char* ));

GRAY_LONG_IMAGE     *allocGrayLongImage __P(( int, int ));
void            freeGrayLongImage __P(( GRAY_LONG_IMAGE* ));

GRAY_DOUBLE_IMAGE   *allocGrayDoubleImage __P(( int, int ));
void            freeGrayDoubleImage __P(( GRAY_DOUBLE_IMAGE* ));

RGB_PACKED_IMAGE    *allocRGBPackedImage __P(( int, int ));
void            freeRGBPackedImage __P(( RGB_PACKED_IMAGE* ));
RGB_PACKED_IMAGE    *readRGBPackedImage __P(( char* ));
int         writeRGBPackedImage __P(( RGB_PACKED_IMAGE*, char* ));
RGB_PACKED_IMAGE    *copyRGBPackedImage __P(( RGB_PACKED_IMAGE* ));

RGB_PLANAR_IMAGE    *allocRGBPlanarImage __P(( int, int ));
void            freeRGBPlanarImage __P(( RGB_PLANAR_IMAGE* ));

int         setError __P(( int ));
void            clearError __P(( void ));
int         checkError __P(( void ));
void            printError __P(( char* ));

void    setRGBMarkColor __P(( int, int, int ));
void    markDotRGBPackedImage __P(( RGB_PACKED_IMAGE*, int, int ));
void    drawLineRGBPackedImage __P(( RGB_PACKED_IMAGE*, int, int, int, int ));
void    drawPolygonRGBPackedImage __P(( RGB_PACKED_IMAGE*, int*, int*, int ));
void    drawRotatedRectangleRGBPackedImage __P(( RGB_PACKED_IMAGE*,
        int, int, int, int, int ));

