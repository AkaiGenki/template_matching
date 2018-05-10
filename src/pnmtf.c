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

/*
    背景画素を除去してなるべく画像サイズを小さくする
    cx, cyは元画像の中心座標に対応する背景画素除去後の画像の座標
 */
#ifdef __STDC__
RGB_PACKED_IMAGE *
removeBackGroundColor( RGB_PACKED_IMAGE *img, int *cx, int *cy )
#else
RGB_PACKED_IMAGE *
removeBackGroundColor( img )
RGB_PACKED_IMAGE *img ;
int *cx;
int *cy;
#endif
{
    RGB_PACKED_IMAGE *res ;
    int i, j, ii, jj;
    int xmin, ymin, xmax, ymax;
    int cols, rows;
    
    /* 
        画素が背景でない座標について，
        x座標とy座標の最小値と最大値をそれぞれ求める
     */
    xmin = img->cols;
    ymin = img->rows;
    xmax = 0;
    ymax = 0;
    for (i = 0; i < img->rows; i++) {
        for (j = 0; j < img->cols; j++) {
            // 背景画素ではない
            if (img->p[i][j].r != 255 || img->p[i][j].g != 255 || img->p[i][j].b != 255) {
                if (xmin > j) xmin = j;
                if (ymin > i) ymin = i;
                if (xmax < j) xmax = j;
                if (ymax < i) ymax = i;
            }
        }
    }

    // 背景画素を除いた画像のサイズを求める
    cols = xmax - xmin + 1;
    rows = ymax - ymin + 1;

    if (cols <= 0 || rows <= 0) {
        printError( "remove" ) ;
        return (0) ;
    }

    if (!( res = allocRGBPackedImage( cols, rows ))) {
        printError( "remove" ) ;
        return (0) ;
    }

    *cx = -1;
    *cy = -1;
    // 背景画素を除いた画像の作成
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            // 背景画素を除いた画像の座標に対応する元画像の座標を求める
            ii = ymin + i;
            jj = xmin + j;

            if (ii == img->rows / 2 && jj == img->cols / 2) {
                *cx = j;
                *cy = i;
            }

            res->p[i][j].r = img->p[ii][jj].r;
            res->p[i][j].g = img->p[ii][jj].g;
            res->p[i][j].b = img->p[ii][jj].b;
        }
    }
    return res;
}
