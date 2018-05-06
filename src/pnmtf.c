#include <stdio.h>
#include "pnmimg.h"

/*
    画像を反時計回りにrotation(度)回転させ，scaling倍に拡大する
 */
#ifdef __STDC__
RGB_PACKED_IMAGE *
affine( RGB_PACKED_IMAGE *img, double rotation, double scaling )
#else
RGB_PACKED_IMAGE *
affine( img, rotation, scaling )
RGB_PACKED_IMAGE *img ;
double rotation ;
double scaling ;
#endif
{
    RGB_PACKED_IMAGE *res ;
    int cols, rows, i, j, ii, jj ;
    double dr, sinR, cosR ;
    double sin(), cos(), fabs() ;

    dr = rotation * 3.14159265358979323846 / 180.0 ;
    sinR = sin ( dr ) ;
    cosR = cos ( dr ) ;

    cols = (int)((fabs(img->cols * cosR) + fabs(img->rows * sinR)) * scaling);
    rows = (int)((fabs(img->cols * sinR) + fabs(img->rows * cosR)) * scaling);

    if (!( res = allocRGBPackedImage( cols, rows ))) {
        printError( "affine" ) ;
        return (0) ;
    }

	for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            ii = (int)((sinR * (j - cols / 2) + cosR * (i - rows / 2)) / scaling + img->rows / 2 + 0.5);
            jj = (int)((cosR * (j - cols / 2) - sinR * (i - rows / 2)) / scaling + img->cols / 2 + 0.5);
            if (ii < 0 || ii >= img->rows || jj < 0 || jj >= img->cols) {
                res->p[i][j].r = 255;
                res->p[i][j].g = 255;
                res->p[i][j].b = 255;
            } else {
                res->p[i][j].r = img->p[ii][jj].r;
                res->p[i][j].g = img->p[ii][jj].g;
                res->p[i][j].b = img->p[ii][jj].b;
            }
        }
    }

    return res;
}
